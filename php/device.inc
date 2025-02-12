<?php

function get_dev_stage_settings( $device )
{
  $d = array();
  $dprop = get_properties($device,'device');
  $d['deviceid'] = $device;
  /// ID within the stage, typically a number from 0 to number of stage devices:
  $d['id'] = intval($dprop['chair']);
  /// Label of the stage device:
  $d['label'] = $dprop['label'];
  /// List of channels the device is providing:
  $d['channels'] = $dprop['inputchannels'];
  /// Position of the stage device in the virtual space:
  $d['position'] = $dprop['position'];
  /// Orientation of the stage device in the virtual space, ZYX Euler angles:
  $d['orientation'] = $dprop['orientation'];
  /// Linear gain of the stage device:
  $d['gain'] = pow(10.0,0.05*$dprop['playbackgain']);
  /// Mute flag:
  $d['mute'] = false;
  $d['jitter'] = array('send'=>floatval($dprop['jittersend']),'receive'=>floatval($dprop['jitterreceive']));
  $d['sendlocal'] = boolval($dprop['sendlocal']);
  // settings used only by configuration server:
  $d['useproxy'] = boolval($dprop['useproxy']);
  $d['isproxy'] = boolval($dprop['isproxy']);
  $d['localip'] = $dprop['localip'];
  $d['externalip'] = $dprop['externalip'];
  $d['receivedownmix'] = boolval($dprop['receivedownmix']);
  $d['senddownmix'] = boolval($dprop['senddownmix']);
  $d['usetcptunnel'] = boolval($dprop['usetcptunnel']);
  $d['encryption'] = boolval($dprop['encryption']);
  $d['encryptstate'] = intval($dprop['encryptstate']);
  $d['nozita'] = boolval($dprop['nozita']);
  $d['hiresping'] = boolval($dprop['hiresping']);
  return $d;
}

function cleanup_rooms()
{
  $rooms = list_db('room');
  foreach( list_db('room') as $room=>$rprop ){
    if( $rprop['clearowned'] ){
      // clear owned room, all except own device must go if inactive
      // for more than 2 minutes:
      $devs = get_devices_in_room($room);
      $kick = false;
      if( !empty($devs) ){
        foreach( $devs as $dev ){
          $dprop = get_properties( $dev, 'device' );
          if( ($dprop['owner'] != $rprop['owner']) && ($dprop['age'] > 120 ) ){
            modify_device_prop( $dev, 'room', '');
            $kick = true;
          }
        }
        if( $kick && $rprop['editable'] ){
          unset($rprop['rvbgain']);
          unset($rprop['rvbabs']);
          unset($rprop['rvbdamp']);
          unset($rprop['size']);
          unset($rprop['ambientsound']);
          unset($rprop['ambientlevel']);
          unset($rprop['lock']);
          set_properties( $room, 'room', $rprop );
        }
      }
    }else{
      if( empty($rprop['owner']) && ($rprop['enterage']>3600)){
        // only operate on unmanaged rooms which were not entered within the last hour:
        $devs = get_devices_in_room($room);
        $age = 4000;
        if( !empty($devs) ){
          foreach( $devs as $dev ){
            $dprop = get_properties( $dev, 'device' );
            $age = min($age,$dprop['age']);
          }
          // if all devices are switched off for more than an hour or if
          // they entered more than eight hours ago, kick them:
          if( ($age > 3600)||($rprop['enterage']>8*3600) ){
            foreach( $devs as $dev )
              modify_device_prop( $dev, 'room', '');
            if( $rprop['editable'] ){
              unset($rprop['rvbgain']);
              unset($rprop['rvbabs']);
              unset($rprop['rvbdamp']);
              unset($rprop['size']);
              unset($rprop['ambientsound']);
              unset($rprop['ambientlevel']);
              unset($rprop['lock']);
              set_properties( $room, 'room', $rprop );
            }
          }
        }
        // clear bulletin board when entered at least 4 hours ago:
        if( ($rprop['enterage']>4*3600) && empty($devs) ){
          modify_room_prop( $room, 'bulletinboard', '' );
        }
      }
    }
  }
}

function update_room_lat( $host, $port, $src, $dest, $lat, $jit )
{
  $room = $host . ':' . $port;
  $dprop = get_room_prop( $room );
  $latdb = $dprop['lat'];
  $latdb[$src.'-'.$dest] = array('lat'=>$lat,'jit'=>$jit,'access'=>time());
  $dprop['lat'] = $latdb;
  set_properties( $room, 'room', $dprop );
}

function update_room( $host, $port, $name, $pin, $group, $version, $srvjit = -1 )
{
  $room = $host . ':' . $port;
  $dprop = get_room_prop( $room );
  //if( !empty( $name ) )
  //  $dprop['label'] = $name;
  if( empty($dprop['group']) )
    $dprop['group'] = $group;
  $dprop['pin'] = $pin;
  $dprop['access'] = time();
  $dprop['srvjit'] = $srvjit;
  $dprop['version'] = $version;
  set_properties( $room, 'room', $dprop );
  cleanup_rooms();
}

// this is the main function for providing device configuration
function get_room_session( $device, $devhash )
{
  $devprop = get_properties($device,'device');
  $rendersettings = get_dev_stage_settings( $device );
  // relevant settings:
  if( $devprop['selfmonitor'] )
    $rendersettings['egogain'] = pow(10.0,0.05*$devprop['egogain']);
  else
    $rendersettings['egogain'] = 0;
  $rendersettings['outputport1']=$devprop['outputport1'];
  $rendersettings['outputport2']=$devprop['outputport2'];
  $xp = json_decode($devprop['xport']);
  $nxp = array();
  if( is_object( $xp ) ){
    foreach($xp as $src=>$dest){
      $nxp[] = array($src,$dest);
    }
  }
  $rendersettings['xport'] = $nxp;
  $rendersettings['peer2peer'] = boolval($devprop['peer2peer']);
  $rendersettings['usetcptunnel'] = boolval($devprop['usetcptunnel']);
  $rendersettings['encryption'] = boolval($devprop['encryption']);
  $rendersettings['renderreverb'] = boolval($devprop['reverb']);
  $rendersettings['renderism'] = boolval($devprop['renderism']);
  $rendersettings['outputgain'] = pow(10.0,0.05*$devprop['mastergain']);
  $rendersettings['mastergain'] = $rendersettings['outputgain'];
  $rendersettings['rectype'] = $devprop['rectype'];
  $rendersettings['decorr'] = floatval($devprop['decorr']);
  $rendersettings['secrec'] = floatval($devprop['secrec']);
  $rendersettings['rawmode'] = boolval($devprop['rawmode']);
  $rendersettings['receive'] = boolval($devprop['receive']);
  $rendersettings['distancelaw'] = boolval($devprop['distancelaw']);
  $rendersettings['delaycomp'] = floatval($devprop['delaycomp']);
  $rendersettings['xrecport'] = $devprop['xrecport'];
  // old headtracking interface:
  $rendersettings['headtracking'] = boolval($devprop['headtracking']);
  $rendersettings['headtrackingserial'] = boolval($devprop['headtrackingserial']);
  $rendersettings['headtrackingrot'] = boolval($devprop['headtrackingrot']);
  $rendersettings['headtrackingrotsrc'] = boolval($devprop['headtrackingrotsrc']);
  $rendersettings['headtrackingport'] = intval($devprop['headtrackingport']);
  $rendersettings['lmetertc'] = floatval($devprop['lmetertc']);
  $rendersettings['lmeterfw'] = strval($devprop['lmeterfw']);
  $rendersettings['stagedevid'] = 0;
  $audio = array();
  $audio['driver'] = 'jack';
  if( $devprop['jackplugdev'] && (($devprop['jackdevice'] != 'highest')||(version_compare("ovclient-0.5.11-8cc47fd",$devprop['version'])<=0)) && ($devprop['jackdevice'] != 'manual'))
    $audio['device'] = 'plug'.$devprop['jackdevice'];
  else
    $audio['device'] = $devprop['jackdevice'];
  $audio['srate'] = floatval($devprop['jackrate']);
  $audio['periodsize'] = intval($devprop['jackperiod']);
  $audio['numperiods'] = intval($devprop['jackbuffers']);
  $audio['priority'] = intval($devprop['jackpriority']);
  $audio['restart'] = boolval($devprop['jackrestart']);
  if( $devprop['jackrestart'] )
    modify_device_prop( $device, 'jackrestart', false );
  $room = array('host'=>'','port'=>0,'pin'=>0);
  $roomdev = array();
  $room['reverb']= array('absorption'=>floatval($devprop['snmon_rvb_abs']),
                         'damping'=>floatval($devprop['snmon_rvb_damp']),
                         'roomgain'=>0.4,
                         'devgain'=>pow(10.0,0.05*($devprop['rvbgain'])),
                         'gain'=>pow(10.0,0.05*($devprop['rvbgain'])));
  $room['size'] = array('x'=>floatval($devprop['snmon_rvb_sx']),'y'=>floatval($devprop['snmon_rvb_sy']),'z'=>floatval($devprop['snmon_rvb_sz']));
  if( !empty($devprop['room']) ){
    $roomprop = get_room_prop( $devprop['room'] );
    if( $roomprop['age'] < 3600 ) {
      $rendersettings['stagedevid'] = intval($devprop['chair']);
      $room['size'] = array('x'=>floatval($roomprop['sx']),'y'=>floatval($roomprop['sy']),'z'=>floatval($roomprop['sz']));
      $roomurl = explode( ":", $devprop['room'] );
      $room['host'] = strval($roomurl[0]);
      $room['port'] = intval($roomurl[1]);
      $room['pin'] = intval($roomprop['pin']);
      $room['ambientsound'] = $roomprop['ambientsound'];
      $room['ambientlevel'] = floatval($roomprop['ambientlevel']);
      $room['reverb']= array('absorption'=>floatval($roomprop['rvbabs']),
                             'damping'=>floatval($roomprop['rvbdamp']),
                             'gaindev'=>pow(10.0,0.05*($devprop['rvbgain'])),
                             'gainroom'=>pow(10.0,0.05*($roomprop['rvbgain'])),
                             'roomgain'=>pow(10.0,0.05*($roomprop['rvbgain'])),
                             'devgain'=>pow(10.0,0.05*($devprop['rvbgain'])),
                             'gain'=>pow(10.0,0.05*($roomprop['rvbgain']+$devprop['rvbgain'])));
      // get all devices in room:
      $rdevs = get_devices_in_room( $devprop['room'] );
      foreach( $rdevs as $rdev ){
        $roomdev[] = get_dev_stage_settings( $rdev );
      }
    }
  }
  $proxyip = '';
  // proxy settings:
  if( $devprop['useproxy'] ){
    // get potential proxies. Must be
    // - in same session
    // - offer proxy service
    // - directly connected to the internet or in same network
    // devices of same session
    foreach( $roomdev as $rdev ){
      if( $rdev['isproxy'] && !empty($rdev['localip']) && !empty($rdev['externalip']) ){
        // offers proxy service and has a configured IP address
        if( ($rdev['localip']==$rdev['externalip'])||($rdev['externalip']==$devprop['externalip']) ){
          // is exposed to internet (externalip==localip) or has same external IP as this device
          if( $rdev['localip'] != $devprop['localip'] ){
            // this is not the same device as this
            $proxyip = $rdev['localip'];
          }
        }
      }
    }    
  }
  $proxyclients = array();
  if( $devprop['isproxy'] ){
    // get list of clients (ip/stage id) who use this device as proxy:
    foreach( $roomdev as $rdev ){
      if( $rdev['useproxy'] == $device ){
        $proxyclientsettings = get_properties( $rdev['deviceid'], 'device' );
        $proxyclients[] = array('id'=>$rdev['id'],'ip'=>$proxyclientsettings['localip']);
      }
    }
  }
  $rendersettings['extracfg'] = array('headtrack'=>array('load'=>boolval($devprop['headtracking']),
                                                         'serial'=>boolval($devprop['headtrackingserial']),
                                                         'applyrec'=>boolval($devprop['headtrackingrot']),
                                                         'applysrc'=>boolval($devprop['headtrackingrotsrc']),
                                                         'port'=>intval($devprop['headtrackingport']),
                                                         'tauref'=>floatval($devprop['headtrackingtauref']),
                                                         'tilturl'=>$devprop['headtrackingtilturl'],
                                                         'tiltpath'=>$devprop['headtrackingtiltpath'],
                                                         'eogpath'=>$devprop['headtrackingeogpath'],
                                                         'autorefzonly'=>boolval($devprop['headtrackingautorefzonly']),
                                                         'tiltmap'=>$devprop['headtrackingtiltmap']),
                                      'monitor'=>array('delay'=>floatval($devprop['selfmonitordelay']),
                                                       'active'=>boolval($devprop['selfmonitor']),
                                                       'onlyreverb'=>boolval($devprop['selfmonitoronlyreverb'])),
                                      'metronome'=>array('active'=>boolval($devprop['metroactive']),
                                                         'bpm'=>floatval($devprop['metrobpm']),
                                                         'bpb'=>intval($devprop['metrobpb']),
                                                         'level'=>floatval($devprop['metrolevel']),
                                                         'delay'=>floatval($devprop['metrodelay'])),
                                      'tscinclude'=>$devprop['tscinclude'],
                                      'mhaconfig'=>$devprop['mhaconfig'],
                                      'network'=>array('expeditedforwarding'=>boolval($devprop['expeditedforwarding']),
                                                       'sessionportoffset'=>intval($devprop['sessionportoffset'])),
                                      'proxy'=>array('isproxy'=>boolval($devprop['isproxy']),
                                                     'useproxy'=>!empty($proxyip),
                                                     'proxyip'=>$proxyip,
                                                     'clients'=>$proxyclients),
                                      'jackrec'=>array('sampleformat'=>$devprop['jackrecsampleformat'],
                                                       'fileformat'=>$devprop['jackrecfileformat']),
                                      'mcrec'=>array('use'=>boolval($devprop['uselocmcrec']),
                                                     'usesender'=>boolval($devprop['uselocmcsend']),
                                                     'addr'=>$devprop['locmcrecaddr'],
                                                     'port'=>intval($devprop['locmcrecport']),
                                                     'device'=>$devprop['locmcrecdevice'],
                                                     'buffer'=>intval($devprop['locmcrecbuffer']),
                                                     'channels'=>$devprop['locmcrecchannels'],
                                                     'autoconnectrec'=>boolval($devprop['locmcrecautoconnect']),
                                                     'sendchannels'=>intval($devprop['locmcsendchannels'])),
                                      'render'=>array('soundscape'=>boolval($devprop['rendersoundscape']),
                                                      'useloudspeaker'=>boolval($devprop['useloudspeaker']),
                                                      'fdnforwardstages'=>intval($devprop['fdnforwardstages']),
                                                      'fdnorder'=>intval($devprop['fdnorder']),
                                                      'emptysessionismonitor'=>boolval($devprop['emptysessionismonitor']),
                                                      'echoc_level'=>floatval($devprop['echoc_level']),
                                                      'echoc_nrep'=>intval($devprop['echoc_nrep']),
                                                      'echoc_filterlen'=>intval($devprop['echoc_filterlen']),
                                                      'echoc_maxdist'=>floatval($devprop['echoc_maxdist']),
                                                      'usebcf2000'=>boolval($devprop['usebcf2000']),
                                                      'zitasampleformat'=>$devprop['zitasampleformat']));
  $stagecfg = array('rendersettings'=>$rendersettings,
                    'audiocfg'=>$audio,
                    'room'=>$room,
                    'owner'=>$devprop['owner'],
                    'roomdev'=>$roomdev,
                    'firmwareupdate'=>$devprop['firmwareupdate'],
                    'installopenmha'=>$devprop['installopenmha'],
                    'usedevversion'=>$devprop['usedevversion']);
  if( $devprop['firmwareupdate'] || $devprop['installopenmha']){
    modify_device_prop( $device, 'firmwareupdate', false );
    modify_device_prop( $device, 'installopenmha', false );
    modify_device_prop( $device, 'isupdating', true );
  }else
    if( $devprop['isupdating'] )
      modify_device_prop( $device, 'isupdating', false );
  if( $devprop['usedevversion'] )
    modify_device_prop( $device, 'usedevversion', false );
  if( !empty($devprop['lastfrontendconfig']) ){
    modify_device_prop( $device, 'lastfrontendconfig', array() );
  }
  if( !empty($devprop['frontendconfig']) ){
    $stagecfg['frontendconfig'] = $devprop['frontendconfig'];
    modify_device_prop( $device, 'frontendconfig', array() );
    modify_device_prop( $device, 'lastfrontendconfig', $devprop['frontendconfig'] );
  }
  if( $devprop['wifiupdate'] ){
    if($devprop['wifi']){
      if( !empty($devprop['wifissid']) )
        $stagecfg['wifisettings'] = array('ssid'=>$devprop['wifissid'],'passwd'=>$devprop['wifipasswd']);
    }else{
      $stagecfg['wifisettings'] = array('ssid'=>'','passwd'=>'');
    }
    modify_device_prop( $device, 'wifiupdate', false );
  }
  $stagecfg['usehifiberry'] = $devprop['usehifiberry'];
  if( !empty($stagecfg['usehifiberry']) )
    modify_device_prop( $device, 'usehifiberry', '' );
  //if( $devprop['wifipasswd'] != '*****' )
  //  modify_device_prop( $device, 'wifipasswd', '*****' );
  if( $devprop['proxyip'] != $proxyip )
    modify_device_prop( $device, 'proxyip', $proxyip );
  $stagecfg = json_encode( $stagecfg );
  $hash = hash('md5',$stagecfg);
  if( $hash != $devhash ){
    echo $hash . "\n";
    echo $stagecfg;
  }
  //cleanup_rooms();
}

function get_client_ip() {
  $ipaddress = '';
  if (isset($_SERVER['HTTP_CLIENT_IP']) && filter_var($_SERVER['HTTP_CLIENT_IP'],FILTER_VALIDATE_IP,FILTER_FLAG_IPV4))
    $ipaddress = $_SERVER['HTTP_CLIENT_IP'];
  else if(isset($_SERVER['HTTP_X_FORWARDED_FOR']) && filter_var($_SERVER['HTTP_X_FORWARDED_FOR'],FILTER_VALIDATE_IP,FILTER_FLAG_IPV4))
    $ipaddress = $_SERVER['HTTP_X_FORWARDED_FOR'];
  else if(isset($_SERVER['HTTP_X_FORWARDED']) && filter_var($_SERVER['HTTP_X_FORWARDED'],FILTER_VALIDATE_IP,FILTER_FLAG_IPV4))
    $ipaddress = $_SERVER['HTTP_X_FORWARDED'];
  else if(isset($_SERVER['HTTP_FORWARDED_FOR']) && filter_var($_SERVER['HTTP_FORWARDED_FOR'],FILTER_VALIDATE_IP,FILTER_FLAG_IPV4))
    $ipaddress = $_SERVER['HTTP_FORWARDED_FOR'];
  else if(isset($_SERVER['HTTP_FORWARDED']) && filter_var($_SERVER['HTTP_FORWARDED'],FILTER_VALIDATE_IP,FILTER_FLAG_IPV4))
    $ipaddress = $_SERVER['HTTP_FORWARDED'];
  else if(isset($_SERVER['REMOTE_ADDR']) && filter_var($_SERVER['REMOTE_ADDR'],FILTER_VALIDATE_IP,FILTER_FLAG_IPV4))
    $ipaddress = $_SERVER['REMOTE_ADDR'];
  else
    $ipaddress = 'UNKNOWN';
  return $ipaddress;
}

function clear_room_lat( $host, $port )
{
  $room = $host . ':' . $port;
  $dprop = get_room_prop( $room );
  $dprop['lat'] = array();
  set_properties( $room, 'room', $dprop );
}

/*
 * Local Variables:
 * c-basic-offset: 2
 * mode: php
 * End:
 */

?>
