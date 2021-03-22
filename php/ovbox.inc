<?php

$fp_dev = fopen("../lock_dev.txt", "a+");
$fp_user = fopen("../lock_user.txt", "a+");
$fp_register = fopen("../lock_reg.txt", "a+");
$fp_authfail = fopen("../lock_auth.txt", "a+");

function defaults( $type )
{
  $defaults = array();
  $defaults['sortkey'] = array(
    'user'=>'id',
    'room'=>'id',
    'device'=>'id',
  );
  $defaults['config'] = array(
    'mail'=>'',
    'admin'=>array(),
    'pagetitle'=>'House of Consort',
    'basecss'=>'ovbox.css',
    'credits'=>'Background image by Johannes Gontarski.',
    'moderation'=>false,
  );
  $defaults['style'] = array(
    'credits'=>'',
    'css'=>'ovbox.css',
    'pagetitle'=>'House of Consort',
    'showdonate'=>true,
  );
  $defaults['device'] = array(
    'inittime'=>'',
    'access'=>0,
    'onlineaccess'=>0,
    'chair'=>-1,
    'label'=>'',
    'owner'=>'',
    'version'=>'',
    'selfmonitor'=>true,
    'egogain'=>0,
    'selfmonitordelay'=>0,
    'host'=>'',
    'localip'=>'',
    'inputchannels'=>array(array(
      'sourceport'=>'system:capture_1',
      'gain'=>1,
      'position'=>array('x'=>0,'y'=>0,'z'=>0),
      'directivity'=>'omni',
    )),
    'jitterreceive'=>5,
    'jittersend'=>5,
    'outputport1'=>'system:playback_1',
    'outputport2'=>'system:playback_2',
    'xport'=>'{}',
    'peer2peer'=>true,
    'secrec'=>0,
    'xrecport'=>array(),
    'position'=>array('x'=>0,'y'=>0,'z'=>0),
    'orientation'=>array('z'=>0,'y'=>0,'x'=>0),
    'srcshiftxyz'=>'0 0 0',
    'rawmode'=>false,
    'reverb'=>true,
    'renderism'=>false,
    'rvbgain'=>0,
    'mastergain'=>0,
    'playbackgain'=>0,
    'room'=>'',
    'roomenter'=>0,
    'srcdist'=>0.7,
    'rectype'=>'hrtf',
    'jackdevice'=>'highest',
    'jackplugdev'=>false,
    'jackrate'=>48000,
    'jackperiod'=>96,
    'jackbuffers'=>2,
    'jackrestart'=>false,
    'alsadevs'=>array(),
    'bandwidth'=>array('tx'=>0,'rx'=>0),
    'cpuload'=>0,
    'headtracking'=>false,
    'headtrackingrot'=>true,
    'headtrackingrotsrc'=>true,
    'headtrackingport'=>0,
    'headtrackingtauref'=>33.315,
    'sendlocal'=>true,
    'hwinputchannels'=>array('system:capture_1','system:capture_2'),
    'firmwareupdate'=>false,
    'usedevversion'=>false,
    'isovbox'=>true,
    'frontendconfig', array(),
    'message'=>'',
    'metrobpm'=>120,
    'metrobpb'=>4,
    'metrodelay'=>40,
    'metroactive'=>false,
    'metrolevel'=>55,
    'preset'=>'',
    'isproxy'=>false,
    'useproxy'=>false,
  );
  $defaults['room'] = array(
    'access'=>0,
    'label'=>'',
    'owner'=>'',
    'group'=>'',
    'lat'=>array(),
    'host'=>'',
    'lock'=>false,
    'pin'=>0,
    'port'=>'',
    'private'=>false,
    'rvbabs'=>0.6,
    'rvbdamp'=>0.7,
    'rvbgain'=>-8,
    'size'=>'25 13 7.5',
    'editable'=>true,
    'deventer'=>0,
    'srvjit'=>-1,
    'ambientsound'=>'',
    'ambientlevel'=>50,
    'bulletinboard'=>'',
  );
  $defaults['user'] = array(
    'group'=>'',
    'seesall'=>true,
    'maingroup'=>'',
    'access'=>0,
    'agreedterms'=>false,
    'agreedprivacy'=>false,
    'validpw'=>true,
    'mail'=>'',
    'devverwarn'=>0,
  );
  $defaults['group'] = array( 'style'=>'default' );
  if( array_key_exists( $type, $defaults ) )
    return $defaults[$type];
  return array();
}

function secondsToTime($inputSeconds) {
  $secondsInAMinute = 60;
  $secondsInAnHour  = 60 * $secondsInAMinute;
  $secondsInADay    = 24 * $secondsInAnHour;
  // extract days
  $days = floor($inputSeconds / $secondsInADay);
  // extract hours
  $hourSeconds = $inputSeconds % $secondsInADay;
  $hours = floor($hourSeconds / $secondsInAnHour);
  // extract minutes
  $minuteSeconds = $hourSeconds % $secondsInAnHour;
  $minutes = floor($minuteSeconds / $secondsInAMinute);
  // extract the remaining seconds
  $remainingSeconds = $minuteSeconds % $secondsInAMinute;
  $seconds = ceil($remainingSeconds);
  // return the final array
  $obj = array(
    'd' => (int) $days,
    'h' => (int) $hours,
    'm' => (int) $minutes,
    's' => (int) $seconds,
  );
  return $obj;
}

function numage2str( $nage )
{
  $d = secondsToTime(  $nage );
  $age = '';
  if( $nage > 3600*24*365*40 )
    return 'never';
  if( $nage >= 3600*24 )
    $age = $age . $d['d'] . 'd';
  if( ($nage >= 3600) && ($nage < 7*3600*24) )
    $age = $age . $d['h'] . 'h';
  if( ($nage >= 60) && ($nage < 3600*24) )
    $age = $age . $d['m'] . '\'';
  if( ($nage >= 0) && ($nage < 3600) )
    $age = $age . $d['s'] . '"';
  if( $nage < 0 )
    $age = $nage . 's';
  return $age;
}

function postproc_prop( &$prop, $type )
{
  $prop['now'] = time();
  if( array_key_exists('access',$prop) )
    $prop['age'] = time()-$prop['access'];
  if( array_key_exists('onlineaccess',$prop) )
    $prop['onlineage'] = time()-$prop['onlineaccess'];
  if( $type == 'device' ){
    foreach(array_keys($prop) as $key){
      if( is_numeric($key) )
        unset($prop[$key]);
    }
    if( empty($prop['label']) && (!empty($prop['owner'])))
      $prop['label'] = $prop['owner'];
    $prop['label'] = preg_replace('/[^a-zA-Z0-9-_]*/','',$prop['label']);
    $dim = explode(" ",$prop['srcshiftxyz']);
    if( count($dim)==3)
      $prop['srcshift'] = array('x'=>floatval($dim[0]),'y'=>floatval($dim[1]),'z'=>floatval($dim[2]));
    else{
      $prop['srcshift'] = array('x'=>0,'y'=>0,'z'=>0);
    }
    foreach( $prop['inputchannels'] as &$channel ){
      $channel['position'] = array('x'=>floatval($channel['position']['x']),
                                   'y'=>floatval($channel['position']['y']),
                                   'z'=>floatval($channel['position']['z']));
      $channel['gain'] = floatval($channel['gain']);
    }
    // create two-channel input variables for backward compatibility:
    if( count($prop['inputchannels'])>0 )
      $prop['inputport'] = $prop['inputchannels'][0]['sourceport'];
    else
      $prop['inputport'] = '';
    if( count($prop['inputchannels'])>1 )
      $prop['inputport2'] = $prop['inputchannels'][1]['sourceport'];
    else
      $prop['inputport2'] = '';
    $prop['roomage'] = time()-$prop['roomenter'];
  }
  if( $type == 'room' ){
    $prop['enterage'] = time()-$prop['deventer'];
    if( empty($prop['label']) && (!empty($prop['name'])))
      $prop['label'] = $prop['name'];
    unset($prop['name']);
    $dim = explode(" ",$prop['size']);
    if( count($dim)!=3){
      $dim = array(25,13,7.5);
    }
    $rs_x = max($dim[0], 0.01);
    $rs_y = max($dim[1], 0.01);
    $rs_z = max($dim[2], 0.01);
    $prop['volume'] = $rs_x * $rs_y * $rs_z;
    $prop['rvbabs'] = max($prop['rvbabs'],0.01);
    $prop['t60'] = 0.161*$prop['volume']/($prop['rvbabs']*2*($rs_x*$rs_y+$rs_y*$rs_z+$rs_z*$rs_x));
    $prop['area'] = $rs_x*$rs_y;
    $prop['sx'] = $rs_x;
    $prop['sy'] = $rs_y;
    $prop['sz'] = $rs_z;
  }
}

function get_client_ip() {
  $ipaddress = '';
  if (filter_var($_SERVER['HTTP_CLIENT_IP'],FILTER_VALIDATE_IP,FILTER_FLAG_IPV4))
    $ipaddress = $_SERVER['HTTP_CLIENT_IP'];
  else if(filter_var($_SERVER['HTTP_X_FORWARDED_FOR'],FILTER_VALIDATE_IP,FILTER_FLAG_IPV4))
    $ipaddress = $_SERVER['HTTP_X_FORWARDED_FOR'];
  else if(filter_var($_SERVER['HTTP_X_FORWARDED'],FILTER_VALIDATE_IP,FILTER_FLAG_IPV4))
    $ipaddress = $_SERVER['HTTP_X_FORWARDED'];
  else if(filter_var($_SERVER['HTTP_FORWARDED_FOR'],FILTER_VALIDATE_IP,FILTER_FLAG_IPV4))
    $ipaddress = $_SERVER['HTTP_FORWARDED_FOR'];
  else if(filter_var($_SERVER['HTTP_FORWARDED'],FILTER_VALIDATE_IP,FILTER_FLAG_IPV4))
    $ipaddress = $_SERVER['HTTP_FORWARDED'];
  else if(filter_var($_SERVER['REMOTE_ADDR'],FILTER_VALIDATE_IP,FILTER_FLAG_IPV4))
    $ipaddress = $_SERVER['REMOTE_ADDR'];
  else
    $ipaddress = 'UNKNOWN';
  return $ipaddress;
}

function get_properties( $id, $type )
{
  $fname = '../db/'.$id.'.'.$type;
  $prop = defaults($type);
  if( file_exists( $fname ) )
    $prop = array_merge($prop, json_decode( file_get_contents( $fname ), true ));
  postproc_prop( $prop, $type );
  return $prop;
}

function set_properties( $id, $type, $dprop )
{
  $fname = '../db/'.$id.'.'.$type;
  file_put_contents( $fname, json_encode( $dprop ));
}

function print_head( $user = "", $style = "" )
{
  $sitecfg = get_properties('site','config');
  $css = '';
  $title = $sitecfg['pagetitle'];
  if( !empty($style) ){
    $sprop = get_properties( $style, 'style' );
    $css = $sprop['css'];
    $title = $sprop['pagetitle'];
  }
  echo "<!DOCTYPE html>\n";
  echo '<html lang="en"><head><title>'.$title;
  if( !empty($user) ){
    echo ' - ' . $user;
  }
  echo '</title><style>';
  if( file_exists( '../css/'.$sitecfg['basecss'] ) ){
    echo file_get_contents( '../css/'.$sitecfg['basecss'] );
  }
  if( (!empty($css)) && file_exists( '../css/'.$css ) ){
    echo file_get_contents( '../css/'.$css );
  }
  echo '</style>';
  echo '<script>'.file_get_contents('../js/ovbox.js').'</script>'."\n";
  echo '<meta name="viewport" content="width=device-width, initial-scale=1"/>';
  echo '</head><body onload="starttimer();">';
  echo '<h1>'.$title.'</h1>' . "\n";
  echo '<div class="ad">March 26th: <b>ovbox concert</b> <i>Hope of my Heart</i> <a target="blank" href="http://www.orlandoviols.com/">www.orlandoviols.com</a></div>'."\n";
}

function print_foot($style = '',$show_donate = true)
{
  $sitecfg = get_properties('site','config');
  $credits = $sitecfg['credits'];
  if( !empty($style) ){
    $sprop = get_properties( $style, 'style' );
    $credits = $sprop['credits'];
    $show_donate = $sprop['showdonate'];
  }
  if( $show_donate ){
    echo '<div class="donate">';
    echo '<form action="https://www.paypal.com/cgi-bin/webscr" method="post" target="_top">';
    echo '<input type="hidden" name="cmd" value="_s-xclick" />';
    echo '<input type="hidden" name="hosted_button_id" value="V63WV7SAVQ6EC" />';
    echo '<input type="image" src="https://www.paypalobjects.com/en_US/DK/i/btn/btn_donateCC_LG.gif" border="0" name="submit" title="PayPal - The safer, easier way to pay online!" alt="Donate with PayPal button" />';
    echo '<img alt="" border="0" src="https://www.paypal.com/en_DE/i/scr/pixel.gif" width="1" height="1" />';
    echo '</form>';
    echo '</div>';
  }
  echo '<div class="ovstage">A service by <a target="blank" href="https://orlandoviols.com/">ORLANDOviols</a> in cooperation with <a target="blank" href="https://digital-stage.org/">Digital Stage</a>.</div>';
  echo '<div class="foto">'.$credits.'</div>';
  echo '<div class="foto"><a target="blank" href="terms.php">Terms of Service</a> / <a target="blank" href="privacy.php">Privacy Policy</a> / <a target="blank" href="datenschutz.php">Datenschutzerkärung</a></div>';
  echo '</body>';
  echo '</html>';
}

function get_rooms_user( $user, $userprop, $usergroups, $devroom )
{
  $allrooms = list_db('room');
  ksort($allrooms);
  $drooms = array();
  $mgrooms = array();
  $grooms = array();
  $orooms = array();
  foreach( $allrooms as $room=>$rprop){
    if( ($rprop['age'] < 1000) &&
        ((!$rprop['private']) || ($rprop['owner']==$user)) &&
        ($userprop['seesall'] || ($rprop['owner']==$user) || in_array($rprop['group'],$usergroups)) &&
        (empty($rprop['group']) || (in_array($rprop['group'],$usergroups)))){
      $rprop['id'] = $room;
      $rprop['entered'] = $devroom == $room;
      $roomdev = get_devices_in_room( $room, false, true );
      ksort($roomdev);
      if( $rprop['lock'] && empty($roomdev) ){
        modify_room_prop( $room, 'lock', false );
        $rprop['lock'] = false;
      }
      $roomdevp = array();
      $rprop['roomdev'] = $roomdev;
      if( $rprop['entered'] ){
        $drooms[] = $rprop;
      }else{
        $rprop['lat'] = array();
        if( !empty($rprop['group']) ){
          if( $userprop['maingroup'] == $rprop['group'] )
            $mgrooms[] = $rprop;
          else
            $grooms[] = $rprop;
        }else
          $orooms[] = $rprop;
      }
    }
  }
  $allrooms = array();
  foreach( $drooms as $room )
    $allrooms[] = $room;
  foreach( $mgrooms as $room )
    $allrooms[] = $room;
  foreach( $grooms as $room )
    $allrooms[] = $room;
  foreach( $orooms as $room )
    $allrooms[] = $room;
  return $allrooms;
}

function list_styles()
{
  $styles = [];
  foreach( glob( '../db/*.style') as $style){
    $styles[] = basename($style,'.style');
  }
  return $styles;
}

function select_userdev( $user, $dev )
{
  $fname = '../db/' . $user . '.userdevice';
  if( empty($dev) ){
    unlink($fname);
  }else{
    file_put_contents( $fname, $dev );
  }
}

function rm_device( $dev )
{
  $fname = '../db/' . $dev . '.device';
  unlink($fname);
}

function rm_room( $room )
{
  $fname = '../db/' . $room . '.room';
  unlink($fname);
}

function rm_old_rooms()
{
  $allrooms = list_db('room');
  foreach( $allrooms as $room => $rprop ){
    if( ($rprop['age'] > 3600) && empty($rprop['owner']) ){
      rm_room($room);
    }
  }
}

function rm_old_unclaimed_devices()
{
  $users = list_users();
  $alldevs = list_db('device');
  foreach( $alldevs as $dev => $rprop ){
    // operate only on inactive devices:
    if( $rprop['age'] > 3600 ){
      if( empty($rprop['owner']) )
        rm_device($dev);
      else
        if( !in_array($rprop['owner'],$users) )
          rm_device($dev);
    }
  }
}

function get_device( $user )
{
  $device = '';
  $fname = '../db/' . $user . '.userdevice';
  if( file_exists( $fname ) ) {
    $devicefd = fopen( $fname, 'r' );
    if ( $devicefd ) {
      $device = trim(fgets( $devicefd ));
      fclose( $devicefd );
    }
  }
  return $device;
}

function modify_device_prop( $device, $key, $value )
{
  $dprop = get_properties( $device, 'device' );
  $dprop[$key] = $value;
  set_properties( $device, 'device', $dprop );
}

function sort_comp_key( &$db, $sortkey, $sortkey2 = '' )
{
  if( $sortkey == 'version' ){
    uasort( $db, function ( $a, $b ) use ($sortkey, $sortkey2)
    {
      $rv = version_compare( $a[$sortkey], $b[$sortkey] );
      if( $rv != 0 )
        return $rv;
      if( empty( $sortkey2 ) )
        return 0;
      if( $a[$sortkey2] > $b[$sortkey2] )
        return 1;
      if( $a[$sortkey2] < $b[$sortkey2] )
        return -1;
      return 0;
    } );
  }else{
    uasort( $db, function ( $a, $b ) use ($sortkey, $sortkey2)
    {
      if( $a[$sortkey] > $b[$sortkey] )
        return 1;
      if( $a[$sortkey] < $b[$sortkey] )
        return -1;
      if( empty( $sortkey2 ) )
        return 0;
      if( $a[$sortkey2] > $b[$sortkey2] )
        return 1;
      if( $a[$sortkey2] < $b[$sortkey2] )
        return -1;
      return 0;
    } );
  }
}

function list_db($type)
{
  $devs = array();
  foreach( glob( '../db/*.'.$type) as $dev){
    $device = basename($dev,'.'.$type);
    $prop = get_properties($device,$type);
    $devs[$device] = $prop;
  }
  ksort( $devs );
  return $devs;
}

function list_devices()
{
  return list_db('device');
}

function list_unclaimed_devices()
{
  $devs = array();
  foreach( list_devices() as $dev=>$devprop){
    if( ($devprop['age'] < 20) && empty($devprop['owner']) && ($devprop['onlineage'] < 600) ){
      $devs[] = $dev;
    }
  }
  return $devs;
}

function owned_devices( $user, $alldev = null )
{
  if( !isset($alldev) )
    $alldev = list_devices();
  $devs = array();
  foreach( $alldev as $dev=>$devprop ){
    if( $devprop['owner'] == $user )
      $devs[$dev] = $devprop;
  }
  return $devs;
}

function lock_room( $room, $device, $lock )
{
  if( in_array($device, get_devices_in_room( $room ) ) ){
    modify_room_prop( $room, 'lock', $lock );
  }
}

function modify_group_prop( $group, $key, $value )
{
  $dprop = get_properties( $group, 'group' );
  $dprop[$key] = $value;
  set_properties( $group, 'group', $dprop );
}

function modify_user_prop( $user, $key, $value )
{
  $dprop = get_properties( $user, 'user' );
  $dprop[$key] = $value;
  set_properties( $user, 'user', $dprop );
}

function get_room_prop( $room )
{
  return get_properties( $room, 'room' );
}

function modify_room_prop( $room, $key, $value )
{
  $dprop = get_room_prop( $room );
  $dprop[$key] = $value;
  set_properties( $room, 'room', $dprop );
}

function update_room( $host, $port, $name, $pin, $group, $srvjit = -1 )
{
  $room = $host . ':' . $port;
  $dprop = get_room_prop( $room );
  if( empty($dprop['label']) )
    $dprop['label'] = $name;
  if( empty($dprop['group']) )
    $dprop['group'] = $group;
  $dprop['pin'] = $pin;
  $dprop['access'] = time();
  $dprop['srvjit'] = $srvjit;
  set_properties( $room, 'room', $dprop );
  cleanup_rooms();
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

function clear_room_lat( $host, $port )
{
  $room = $host . ':' . $port;
  $dprop = get_room_prop( $room );
  $dprop['lat'] = array();
  set_properties( $room, 'room', $dprop );
}

function issender( $devprop )
{
  return !empty($devprop['inputchannels']);
}

// return a list of all devices (or sending devices only) in a room:
function get_devices_in_room( $room, $senderonly = false, $getprop = false )
{
  $devices = array();
  $nochairdevices = array();
  foreach( glob( '../db/*.device') as $dev){
    $device = basename($dev,'.device');
    $prop = get_properties( $device, 'device' );
    if( $getprop )
      $device = array('id'=>$device,
                      'label'=>strval($prop['label']),
                      'owner'=>strval($prop['owner']),
                      'isactive'=>$prop['age']<20,
                      'issender'=>issender($prop),
                      'jittersend'=>floatval($prop['jittersend']),
                      'jitterreceive'=>floatval($prop['jitterreceive']),
                      'jackrate'=>floatval($prop['jackrate']),
                      'numchannels'=>count($prop['inputchannels']),
                      'peer2peer'=>boolval($prop['peer2peer']));
    if( ($room == $prop['room']) && ((!$senderonly) || issender($prop)) ){
      if( array_key_exists( 'chair', $prop ) )
        $devices[$prop['chair']] = $device;
      else
        $nochairdevices[] = $device;
    }
  }
  foreach( $nochairdevices as $device )
    $devices[] = $device;
  return $devices;
}

function get_free_chair( $room )
{
  $devs = get_devices_in_room( $room );
  $keys = array_keys( $devs );
  $k = 0;
  while( in_array( $k, $keys ) )
    $k++;
  return $k;
}

function set_dev_room_pos( $room )
{
  $roomdev = get_devices_in_room( $room, true );
  ksort($roomdev);
  $stagewidth = 360;
  $az = 0;
  $daz = $stagewidth/count($roomdev)*(pi()/180);
  $az = $az*(pi()/180)-0.5*$daz;
  $radius = 1.2;
  foreach( $roomdev as $chair => $dev ){
    $az = $az+$daz;
    $pos_x = $radius * cos( -$az );
    $pos_y = -$radius * sin( -$az );
    $dprop = get_properties( $dev, 'device' );
    $dprop['position'] = array('x'=>$pos_x,'y'=>$pos_y,'z'=>0);
    $dprop['orientation'] = array('z'=>(180/pi()*($az+pi())),'y'=>0,'x'=>0);
    set_properties( $dev, 'device', $dprop );
  }
}

function device_enter_room( $device, $room )
{
  $othersinroom = get_devices_in_room( $room );
  if( !in_array( $device, $othersinroom ) ){
    $dprop = get_properties( $device, 'device' );
    $dprop['room'] = $room;
    $dprop['chair'] = get_free_chair($room);
    if( empty($room) )
      $dprop['roomenter'] = 0;
    else
      $dprop['roomenter'] = time();
    // store default position and orientation, in case it is not
    // overwritten by set_dev_room_pos:
    $dprop['position'] = array('x'=>0,'y'=>0,'z'=>0);
    $dprop['orientation'] = array('z'=>0,'y'=>0,'x'=>0);
    set_properties( $device, 'device', $dprop );
    set_dev_room_pos( $room );
    modify_room_prop( $room, 'deventer', time() );
  }
}

function room_swap_devices( $dev1, $dev2 )
{
  $dprop = get_properties( $dev1, 'device' );
  $room = $dprop['room'];
  $members = get_devices_in_room( $room );
  $chair1 = -1;
  $chair2 = -1;
  foreach( $members as $key => $dev ){
    if( $dev == $dev1 )
      $chair1 = $key;
    if( $dev == $dev2 )
      $chair2 = $key;
  }
  if( ($chair1>=0) && ($chair2>=0) ){
    modify_device_prop( $dev1, 'chair', $chair2 );
    modify_device_prop( $dev2, 'chair', $chair1 );
    set_dev_room_pos( $room );
  }
}

function xml_add_sound( &$source, $doc, $attrs )
{
  $sound = $doc->createElement("sound");
  $source->appendChild($sound);
  $sound->setAttribute("maxdist","50");
  $sound->setAttribute("gainmodel","1");
  foreach( $attrs as $attr=>$value ){
    $sound->setAttribute($attr,$value);
  }
  return $sound;
}

function get_tascar_cfg( $device, $devhash )
{
  $success = false;
  $waitports = '';
  $localdevprop = get_properties( $device, 'device' );
  $amsender = issender($localdevprop);
  // create TASCAR document:
  $doc = new DOMDocument();
  $session = $doc->createElement("session");
  $doc->appendChild($session);
  $session->setAttribute('duration','36000');
  $session->setAttribute('name',$device);
  $session->setAttribute('license','CC0');
  $session->setAttribute("levelmeter_tc","0.5");
  $scene = $doc->createElement('scene');
  $scene->setAttribute('name',$device);
  $scene->setAttribute('guiscale','25');
  if( ! $localdevprop['rawmode'] ){
    $session->appendChild($scene);
    $session->setAttribute('srv_port','9871');
  }else{
    $session->setAttribute('srv_port','0');
  }
  $modules = $doc->createElement("modules");
  $session->appendChild($modules);
  // add receiver:
  $rec = $doc->createElement("receiver");
  $rec->setAttribute('type',$localdevprop['rectype']);
  if( $localdevprop['rectype'] == 'ortf' ){
    $rec->setAttribute('angle','140');
    $rec->setAttribute('f6db','12000');
    $rec->setAttribute('fmin','3000');
  }
  $rec->setAttribute("name","master");
  $rec->setAttribute("delaycomp","0.05");
  $scene->appendChild($rec);
  // master connections
  if( ! $localdevprop['rawmode'] ){
    $conn = $doc->createElement("connect");
    $conn->setAttribute("src",'render.'.$device.':master_l');
    $conn->setAttribute("dest",$localdevprop['outputport1']);
    if( !empty($localdevprop['outputport1']))
      $session->appendChild($conn);
    $conn = $doc->createElement("connect");
    $conn->setAttribute("src",'render.'.$device.':master_r');
    $conn->setAttribute("dest",$localdevprop['outputport2']);
    if( !empty($localdevprop['outputport2']))
      $session->appendChild($conn);
  }
  // room specific setup:
  if ( !empty( $localdevprop['room'] ) ){
    $room = $localdevprop['room'];
    $roomprop = get_room_prop( $room );
    $mychair = -1;
    if( $roomprop['age'] < 3600 ) {
      // get all sending devives:
      $roomdev = get_devices_in_room( $room );
      foreach( $roomdev as $chair => $dev )
        if ( $dev == $device )
          $mychair = $chair;
      // get all sending devives:
      $roomdev = get_devices_in_room( $room, true );
      // sort by chair number:
      ksort($roomdev);
      // if I am sending sit in circle, otherwise use 120 degree stage
      $stagewidth = 160;
      $az = -80;
      if( $amsender ){
        $stagewidth = 360;
        $az = 0;
      }
      $daz = $stagewidth/count($roomdev)*(pi()/180);
      $az = $az*(pi()/180)-0.5*$daz;
      $radius = 1.2;
      foreach( $roomdev as $chair => $dev ){
        $az = $az+$daz;
        $pos_x = sprintf("%1.3f",$radius * cos( $az ));
        $pos_y = sprintf("%1.3f",-$radius * sin( $az ));
        // get configuration of player device:
        $devprop = get_properties( $dev, 'device' );
        $numsource = count($devprop['inputchannels']);
        if( $numsource > 0 ){
          $dz = 0;
          $dx = 0;
          // relative gain:
          $gain = -4.41;
          $source = $doc->createElement("source");
          $scene->appendChild($source);
          $inport = '';
          if ( $dev == $device ){
            $source->setAttribute("name","ego");
            $dz = -0.3;
            $dx = 0.2;
            $gain = $devprop['egogain'];
            if( !$devprop['selfmonitor'] )
              $source->setAttribute("mute","true");
          }else{
            $source->setAttribute("name",$devprop['label'].'_'.$chair);
          }
          $gain = $gain + $devprop['playbackgain'];
          $dy = $devprop['srcdist'] * ($numsource > 1) * 0.5;
          if( !empty($devprop['inputport']) ){
            $inport = '';
            if ( $dev == $device )
              $inport = $devprop['inputport'];
            $srcshift = $devprop['srcshift'];
            $snd = xml_add_sound( $source, $doc, array('x'=>$dx+$srcshift['x'], 'y'=>$dy+$srcshift['y'], 'z'=>$dz+$srcshift['z'], 'connect'=>$inport, 'gain'=>$gain) );
            if ( $dev == $device ){
              $plugs = $snd->appendChild($doc->createElement('plugins'));
              $sndf = $plugs->appendChild($doc->createElement('sndfile'));
              $sndf->setAttribute('name','akkord_up_48.wav');
              $sndf->setAttribute('level','60');
              $sndf->setAttribute('triggered','true');
              $sndf->setAttribute('transport','false');
              $sndf = $plugs->appendChild($doc->createElement('sndfile'));
              $sndf->setAttribute('name','akkord_down_48.wav');
              $sndf->setAttribute('level','60');
              $sndf->setAttribute('triggered','true');
              $sndf->setAttribute('transport','false');
            }
          }
          if( !empty($devprop['inputport2']) ){
            $inport = '';
            if ( $dev == $device )
              $inport = $devprop['inputport2'];
            $snd = xml_add_sound( $source, $doc, array('x'=>$dx, 'y'=>-$dy, 'z'=>$dz, 'connect'=>$inport, 'gain'=>$gain) );
            if ( $dev == $device ){
              $plugs = $snd->appendChild($doc->createElement('plugins'));
              $sndf = $plugs->appendChild($doc->createElement('sndfile'));
              $sndf->setAttribute('name','akkord_up_48.wav');
              $sndf->setAttribute('level','60');
              $sndf->setAttribute('triggered','true');
              $sndf->setAttribute('transport','false');
              $sndf = $plugs->appendChild($doc->createElement('sndfile'));
              $sndf->setAttribute('name','akkord_down_48.wav');
              $sndf->setAttribute('level','60');
              $sndf->setAttribute('triggered','true');
              $sndf->setAttribute('transport','false');
            }
          }
          $xmlpos = $doc->createElement("position");
          $source->appendChild($xmlpos);
          $xmlpos->appendChild($doc->createTextNode("0 ".$pos_x." ".$pos_y." 0"));
          $xmlrot = $doc->createElement("orientation");
          $source->appendChild($xmlrot);
          $xmlrot->appendChild($doc->createTextNode("0 ".(180/pi()*(-$az+pi()))." 0 0"));
          if ( $dev == $device ){
            $mychair = $chair;
            // set receiver position:
            $xmlpos = $doc->createElement("position");
            $rec->appendChild($xmlpos);
            $xmlpos->appendChild($doc->createTextNode("0 ".$pos_x." ".$pos_y." 0"));
            $xmlrot = $doc->createElement("orientation");
            $rec->appendChild($xmlrot);
            $xmlrot->appendChild($doc->createTextNode("0 ".(180/pi()*(-$az+pi()))." 0 0"));
          }
        }
      }
      if( $localdevprop['reverb'] ){
        // reverb
        $reverb = $doc->createElement("reverb");
        $reverb->setAttribute("type","simplefdn");
        $reverb->setAttribute("volumetric",$roomprop['size']);
        $reverb->setAttribute("image","false");
        $reverb->setAttribute("fdnorder","5");
        $reverb->setAttribute("dw","60");
        $reverb->setAttribute("absorption",$roomprop['rvbabs']);
        $reverb->setAttribute("damping",$roomprop['rvbdamp']);
        $reverb->setAttribute("gain",$roomprop['rvbgain']+$localdevprop['rvbgain']);
        $scene->appendChild($reverb);
      }
      // modules
      $mod = $doc->createElement("system");
      $roomurl = explode( ":", $room );
      $myport = 4464+2*$mychair;
      $p2p = '';
      if( $localdevprop['peer2peer'] )
        $p2p = ' -2';
      if( $localdevprop['secrec'] > 0 )
        $p2p = $p2p . ' -x 100';
      $mod->setAttribute("command","../udpmirror/ovboxclient --rtprio=30 -s ".$roomprop['pin']." -d ". $roomurl[0] . " -p " . $roomurl[1] . " -c " . $mychair . " -l " . $myport . $p2p);
      $mod->setAttribute("onunload","killall ovboxclient ../udpmirror/ovboxclient");
      $modules->appendChild($mod);
      // receiver:
      foreach( $roomdev as $chair => $dev ){
        $devprop = get_properties( $dev, 'device' );
        $devuser = $devprop['label'];
        $numsource = !empty($devprop['inputport']) + !empty($devprop['inputport2']);
        if ( ( $numsource > 0 ) && ($dev != $device) ){
          $iport = 4464+2*$chair;
          $buff = $localdevprop['jitterreceive']+$devprop['jittersend'];
          $chan = '1';
          if( $numsource > 1 )
            $chan = $chan . ',2';
          // zita
          $mod = $modules->appendChild($doc->createElement('system'));
          $mod->setAttribute('command','zita-n2j --chan '.$chan.' --jname ' . $devuser . '_'.$chair.' --buf ' . $buff . ' 0.0.0.0 ' . $iport);
          $mod->setAttribute('onunload','killall zita-n2j');
          if( $localdevprop['secrec'] > 0 ){
            $mod = $modules->appendChild($doc->createElement('system'));
            $port = 'n2j_'.$chair.'_sec';
            $mod->setAttribute('command','zita-n2j --chan '.$chan.' --jname '.$port.' --buf ' . ($buff+$localdevprop['secrec']) . ' 0.0.0.0 ' . ($iport+100));
            $waitports = $waitports . ' ' . $port.':out_1';
            if( $numsource > 1 )
              $waitports = $waitports . ' ' . $port.':out_2';
            $mod = $modules->appendChild($doc->createElement('route'));
            $mod->setAttribute('name',$devuser . '_'.$chair.'_sec');
            $mod->setAttribute('channels',$numsource);
            $mod->setAttribute('gain',$devprop['playbackgain']);
            $mod->setAttribute('connect',$port.':out_[12]');
          }
          // connection
          for($k=0;$k<$numsource;$k++){
            $conn = $doc->createElement('connect');
            $port = $devuser . '_'.$chair.':out_'.($k+1);
            $waitports = $waitports . ' ' . $port;
            $conn->setAttribute('src',$port);
            if( $localdevprop['rawmode'] ){
              $conn->setAttribute('dest',$localdevprop['outputport'.($k+1)]);
              if( !empty($localdevprop['outputport'.($k+1)]))
                $session->appendChild($conn);
            }else{
              $conn->setAttribute('dest','render.'.$device.':' . $devuser . '_'.$chair.'.'.$k.'.0');
              $session->appendChild($conn);
            }
          }
        }
      }
      $numsource = !empty($localdevprop['inputport']) + !empty($localdevprop['inputport2']);
      if( $numsource > 0 ){
        $mod = $doc->createElement('system');
        //<system command=\'zita-j2n --chan 1 --jname sender --16bit 127.0.0.1 ${oport}\' onunload=\'killall zita-j2n\'/>'
        $mod->setAttribute('command','zita-j2n --chan '.$numsource.' --jname sender --16bit 127.0.0.1 ' . $myport);
        $mod->setAttribute('onunload','killall zita-j2n');
        $modules->appendChild($mod);
        if( !empty($localdevprop['inputport']) ){
          $conn = $doc->createElement('connect');
          $conn->setAttribute('src',$localdevprop['inputport']);
          $port = 'sender:in_1';
          $waitports = $waitports . ' ' . $port;
          $conn->setAttribute('dest',$port);
          $session->appendChild($conn);
        }
        if( !empty($localdevprop['inputport2']) ){
          $conn = $doc->createElement('connect');
          $conn->setAttribute('src',$localdevprop['inputport2']);
          $port = 'sender:in_'.$numsource;
          $waitports = $waitports . ' ' . $port;
          $conn->setAttribute('dest',$port);
          $session->appendChild($conn);
        }
        if( $localdevprop['secrec'] > 0 ){
          $mod = $modules->appendChild($doc->createElement('route'));
          $mod->setAttribute('name','egodelayed');
          $mod->setAttribute('channels',$numsource);
          $plgs = $mod->appendChild($doc->createElement('plugins'));
          $del = $plgs->appendChild($doc->createElement('delay'));
          $del->setAttribute('delay',0.001*($localdevprop['secrec']+$localdevprop['jitterreceive']+$localdevprop['jittersend']));
          $mod->setAttribute('gain',$localdevprop['playbackgain']);
          if( !empty($localdevprop['inputport']) ){
            $conn = $doc->createElement('connect');
            $conn->setAttribute('src',$localdevprop['inputport']);
            $conn->setAttribute('dest', 'egodelayed:in.0');
            $session->appendChild($conn);
          }
          if( !empty($localdevprop['inputport2']) ){
            $conn = $doc->createElement('connect');
            $conn->setAttribute('src',$localdevprop['inputport2']);
            $conn->setAttribute('dest','egodelayed:in.'.($numsource-1));
            $session->appendChild($conn);
          }
        }
      }
      $mod = $doc->createElement('system');
      //$mod->setAttribute('command','sleep 2;sleep 2');
      $mod->setAttribute('command','../udpmirror/jackwaitforport' . $waitports . ';sleep 1');
      $modules->appendChild($mod);
      $mod = $doc->createElement('touchosc');
      if( ! $localdevprop['rawmode'] )
        $modules->appendChild($mod);
      $mod = $doc->createElement('system');
      $mod->setAttribute('command','node bridge.js');
      if( ! $localdevprop['rawmode'] )
        $modules->appendChild($mod);
      $cmd = $modules->appendChild($doc->createElement('system'));
      $cmd->setAttribute('command','../headtracker/headtracker');
      $xport = json_decode($localdevprop['xport']);
      foreach( $xport as $xpsrc=>$xpdest ){
        $conn = $session->appendChild($doc->createElement('connect'));
        $conn->setAttribute('src',$xpsrc);
        $conn->setAttribute('dest',$xpdest);
      }
      $rec->setAttribute('gain',$localdevprop['mastergain']);
      $success = true;
    }
  }
  if( !$success ){
    // add announcement source
    $source = $doc->createElement('source');
    $scene->appendChild($source);
    $source->setAttribute('name','announce');
    $egosound = xml_add_sound($source, $doc, array('x'=>4) );
    $plugs = $doc->createElement('plugins');
    $egosound->appendChild($plugs);
    $sndfile = $doc->createElement('sndfile');
    $plugs->appendChild($sndfile);
    $sndfile->setAttribute('name','announce.flac');
    $sndfile->setAttribute('level','57');
    $sndfile->setAttribute('transport','false');
    $sndfile->setAttribute('loop','0');
  }
  $tsc = $doc->saveXML();
  $hash = hash('md5',$tsc);
  if( $hash != $devhash ){
    echo $hash . "\n";
    echo $tsc;
  }
}

function html_device_selector( $user, $currentdevice )
{
  $dprop = get_properties($currentdevice,'device');
  $devs = list_unclaimed_devices();
  $doc = new DOMDocument('1.0');
  $root = $doc->appendChild($doc->createElement('div'));
  $div = $root->appendChild($doc->createElement('div'));
  $div->setAttribute('id','deverror');
  $div->setAttribute('class','deverror');
  $div->setAttribute('style','display: none;');
  if( (!empty($dprop['message']))&&($dprop['age']<3600) ){
    $div->setAttribute('style','display: block;');
    $b = $div->appendChild($doc->createElement('b'));
    $b->appendChild($doc->createTextNode('Device error:'));
    $div->appendChild($doc->createElement('br'));
    $div->appendChild($doc->createTextNode($dprop['message']));
    if( strpos($dprop['message'],'Unable to connect to the JACK server')!==false){
      $div->appendChild($doc->createElement('br'));
      $div->appendChild($doc->createTextNode('Is your sound card connected and configured correctly?'));
    }
  }
  $div = $root->appendChild($doc->createElement('div'));
  $select = $div->appendChild($doc->createElement('select'));
  $select->setAttribute('id','deviceselector');
  $select->setAttribute('class','uibutton');
  $select->setAttribute('oninput','location.href=\'?devselect=\'+event.target.value;');
  $select->appendChild($doc->createElement('option'))->appendChild($doc->createTextNode(' -- please select a device -- '));
  foreach( owned_devices( $user ) as $dev=>$sdprop ){
    $opt = $select->appendChild($doc->createElement('option'));
    $opt->setAttribute('value',$dev);
    if( $currentdevice == $dev )
      $opt->setAttribute('selected','');
    $active = '';
    if( $sdprop['age'] < 20 )
      $active = ' *active*';
    $opt->appendChild($doc->createTextNode($dev.' ('.$sdprop['label'].')'.$active));
  }
  $pres = $div->appendChild($doc->createElement('span'));
  if(!empty($dprop['preset'])){
    $pres->appendChild($doc->createTextNode($dprop['preset']));
    $pres->setAttribute('class','presetspan presetact');
  }
  $inp = $div->appendChild($doc->createElement('input'));
  $inp->setAttribute('type','button');
  $inp->setAttribute('class','roomsettingstoggle uibutton');
  $inp->setAttribute('onclick','toggledisplay(\'devsettings\',\'device settings\');');
  $inp->setAttribute('value','show device settings');
  echo $doc->saveHTML();
}

function auth( $username, $password )
{
  if( $username == 'admin' )
    return false;
  if( $username == 'device' )
    return false;
  if( $username == 'room' )
    return false;
  $fname = '../db/ovpasswd';
  $pws = array();
  if( !file_exists( $fname ) )
    return false;
  $pws = json_decode( file_get_contents( $fname ), true );
  if( !array_key_exists( $username, $pws ) ){
    // try old format:
    $filePasswd = get_htpasswd( '../ovboxusers', $username );
    return htpw_matches($password, $filePasswd);
  }
  return password_verify( $password, $pws[$username] );
}

function grouphash( $group )
{
  if( empty( $group ) )
    return '';
  $g = md5("setgroup".$group);
  return 'g'.substr($g,0,9);
}

function get_group_by_hash( $hash )
{
  $grp = '';
  foreach( list_groups() as $group )
    if( $hash == grouphash($group) )
      $grp = $group;
  return $grp;
}

function list_users()
{
  $users = array();
  $fname = '../ovboxusers';
  if( file_exists( $fname ) ){
    $fcontent = file_get_contents( $fname );
    $lines = explode("\n",$fcontent);
    foreach( $lines as $line ){
      $linet = explode("#",trim($line));
      $line = $linet[0];
      if( !empty($line) ){
        $linet = explode(":",trim($line));
        if( !empty($linet[0]) ){
          if( ($linet[0]!='room') && ($linet[0]!='device') && ($linet[0]!='admin'))
            $users[] = $linet[0];
        }
      }
    }
  }
  $fname = '../db/ovpasswd';
  if( file_exists( $fname ) ){
    $pws = json_decode( file_get_contents( $fname ), true );
    foreach( $pws as $user => $hash )
      $users[] = $user;
  }
  $dbuser = list_db('user');
  $users = array_unique(array_merge($users,array_keys($dbuser)));
  sort($users);
  return $users;
}

function list_groups( $user = "" )
{
  $groups = array();
  $fname = '../db/groups';
  if( file_exists( $fname ) ){
    $fgroups = json_decode( file_get_contents( $fname ), true );
    if( empty($user) ){
      $groups = array_keys($fgroups);
    }else{
      foreach( $fgroups as $group=>$users ){
        if( in_array( $user, $users ) ){
          $groups[] = $group;
        }
      }
    }
  }
  sort($groups);
  return $groups;
}

function add_group( $group )
{
  if( !empty($group) ){
    $fname = '../db/groups';
    $groups = array();
    if( file_exists( $fname ) )
      $groups = json_decode( file_get_contents( $fname ), true );
    if( !array_key_exists( $group, $groups ) ){
      $groups[$group] = array();
      file_put_contents( $fname, json_encode( $groups ));
    }
  }
}

function rm_group( $group )
{
  if( !empty($group) ){
    $fname = '../db/groups';
    $groups = array();
    if( file_exists( $fname ) )
      $groups = json_decode( file_get_contents( $fname ), true );
    if( array_key_exists( $group, $groups ) ){
      unset($groups[$group]);
      file_put_contents( $fname, json_encode( $groups ));
    }
  }
}

function add_user_to_group( $user, $group )
{
  if( !(empty($group)||empty($user)) ){
    $fname = '../db/groups';
    if( file_exists( $fname ) ){
      $groups = json_decode( file_get_contents( $fname ), true );
      if( array_key_exists( $group, $groups ) ){
        $users = $groups[$group];
        if( !in_array( $user, $users ) ){
          $users[] = $user;
          $groups[$group] = $users;
          file_put_contents( $fname, json_encode( $groups ));
        }
      }
    }
  }
}

function remove_user_from_group( $user, $group )
{
  if( !(empty($group)||empty($user)) ){
    $fname = '../db/groups';
    if( file_exists( $fname ) ){
      $groups = json_decode( file_get_contents( $fname ), true );
      if( array_key_exists( $group, $groups ) ){
        $users = $groups[$group];
        $us = array_search( $user, $users );
        if( $us !== false ){
          unset($users[$us]);
          $groups[$group] = $users;
          file_put_contents( $fname, json_encode( $groups ));
        }
      }
    }
  }
}

function cleanup_groups()
{
  $users = list_users();
  $fname = '../db/groups';
  if( file_exists( $fname ) ){
    $groups = json_decode( file_get_contents( $fname ), true );
    foreach( $groups as $group=>&$members ){
      foreach( $members as &$member )
        if( !in_array($member,$users) ){
          if(($key = array_search($member, $members)) !== false) {
            unset($members[$key]);
          }
        }
    }
    file_put_contents( $fname, json_encode( $groups ));
  }
}

function get_group_users( $group )
{
  $users = array();
  if( !empty($group) ){
    $fname = '../db/groups';
    if( file_exists( $fname ) ){
      $groups = json_decode( file_get_contents( $fname ), true );
      if( array_key_exists( $group, $groups ) )
        $users = $groups[$group];
    }
  }
  return $users;
}

function html_admin_db($type, $showfields = array())
{
  $sortprop = get_properties('list','sortkey');
  $devs = list_db($type);
  $sortkey = $sortprop[$type];
  if( $sortkey=='id' )
    ksort($devs);
  else{
    sort_comp_key( $devs, $sortkey );
  }
  $doc = new DOMDocument('1.0');
  $root = $doc->appendChild($doc->createElement('div'));
  $root->appendChild($doc->createElement('a'))->setAttribute('name',$type.'s');
  $h = $root->appendChild($doc->createElement('h2'));
  $h->appendChild($doc->createTextNode($type.'s: '));
  $s = $h->appendChild($doc->createElement('span'));
  $s->setAttribute('style','font-size: small;');
  $s->appendChild($doc->createTextNode('('.count($devs).')'));
  $tab = $root->appendChild($doc->createElement('table'));
  $tr = $tab->appendChild($doc->createElement('tr'));
  $headerfields = array_merge(array('id','age','onlineage'),$showfields,array('owner','label'));
  foreach( $headerfields as $field ){
    table_add_header($tr,$field,$sortkey,$doc,$type);
  }
  $users = list_users();
  foreach( $devs as $dev=>$dprop ){
    $tr = $tab->appendChild($doc->createElement('tr'));
    $age = numage2str($dprop['age']);
    if( $dprop['age'] < 20 )
      $tr->setAttribute('style','background-color: #dcc378;');
    $tr->appendChild($doc->createElement('td'))->appendChild($doc->createTextNode($dev));
    $tr->appendChild($doc->createElement('td'))->appendChild($doc->createTextNode($age));
    $onlineage = numage2str($dprop['onlineage']);
    $tr->appendChild($doc->createElement('td'))->appendChild($doc->createTextNode($onlineage));
    foreach( $showfields as $field ){
      if( $field == 'roomage' )
        $tr->appendChild($doc->createElement('td'))->appendChild($doc->createTextNode(numage2str($dprop[$field])));
      else
        $tr->appendChild($doc->createElement('td'))->appendChild($doc->createTextNode($dprop[$field]));
    }
    // user:
    $td = $tr->appendChild($doc->createElement('td'));
    $inp = $td->appendChild($doc->createElement('select'));
    $inp->setAttribute('oninput','location.href=\'?set'.$type.'owner='.urlencode($dev).'&owner=\'+event.target.value;');
    $opt = $inp->appendChild($doc->createElement('option'));
    $opt->setAttribute('value','');
    $opt->appendChild($doc->createTextNode(' -- unclaimed -- '));
    if( $dprop['owner'] == '' )
      $opt->setAttribute('selected','');
    foreach($users as $us){
      $opt = $inp->appendChild($doc->createElement('option'));
      $opt->setAttribute('value',$us);
      $opt->appendChild($doc->createTextNode($us));
      if( $dprop['owner'] == $us )
        $opt->setAttribute('selected','');
    }
    //label:
    $td = $tr->appendChild($doc->createElement('td'));
    $form = $td->appendChild($doc->createElement('form'));
    $inp = $form->appendChild($doc->createElement('input'));
    $inp->setAttribute('type','hidden');
    $inp->setAttribute('name','set'.$type.'label');
    $inp->setAttribute('value',$dev);
    $inp = $form->appendChild($doc->createElement('input'));
    $inp->setAttribute('type','text');
    $inp->setAttribute('name','label');
    $inp->setAttribute('pattern','[a-zA-Z0-9-_]*');
    $inp->setAttribute('value',$dprop['label']);
    $form->appendChild($doc->createElement('button'))->appendChild($doc->createTextNode('Save'));
    // delete
    $td = $tr->appendChild($doc->createElement('td'));
    $form = $td->appendChild($doc->createElement('form'));
    $inp = $form->appendChild($doc->createElement('input'));
    $inp->setAttribute('type','checkbox');
    $inp->setAttribute('name','rm'.$type);
    $inp->setAttribute('value',$dev);
    $form->appendChild($doc->createElement('button'))->appendChild($doc->createTextNode('Delete'));
  }
  echo $doc->saveHTML() . "\n";
}

function table_add_header( &$tr, $field, $sortkey, &$doc, $type )
{
  $th = $tr->appendChild($doc->createElement('th'));
  if( $field == $sortkey ){
    $th->appendChild($doc->createTextNode('['.$field.']'));
}else{
    $a = $th->appendChild($doc->createElement('a'));
    $a->setAttribute('href','?sortby='.$field.'&category='.$type);
    $a->appendChild($doc->createTextNode($field));
  }
}

function html_admin_rooms()
{
  $showfields = array();
  $showfields[] = 'srvjit';
  $sortprop = get_properties('list','sortkey');
  $sortkey = $sortprop['room'];
  $rooms = list_db('room');
  if( $sortkey == 'id' )
    ksort($rooms);
  else{
    sort_comp_key( $rooms, $sortkey );
  }
  $doc = new DOMDocument('1.0');
  $root = $doc->appendChild($doc->createElement('div'));
  $root->appendChild($doc->createElement('a'))->setAttribute('name','rooms');
  $h = $root->appendChild($doc->createElement('h2'));
  $h->appendChild($doc->createTextNode('rooms: '));
  $s = $h->appendChild($doc->createElement('span'));
  $s->setAttribute('style','font-size: small;');
  $s->appendChild($doc->createTextNode('('.count($rooms).')'));
  $tab = $root->appendChild($doc->createElement('table'));
  $tr = $tab->appendChild($doc->createElement('tr'));
  // room id:
  $headerfields = array_merge(array('id','age','deventer'),$showfields,array('owner','group','label'));
  foreach( $headerfields as $field )
    table_add_header($tr,$field,$sortkey,$doc,'room');
  $users = list_users();
  $groups = list_groups();
  $cnt_old = 0;
  foreach( $rooms as $dev=>$dprop ){
    $tr = $tab->appendChild($doc->createElement('tr'));
    if( $dprop['age'] > 3600 ){
      if( empty($dprop['owner']) ){
        $tr->setAttribute('style','background-color: #ffaaaa;');
        $cnt_old = $cnt_old+1;
      }
    }
    $tr->appendChild($doc->createElement('td'))->appendChild($doc->createTextNode($dev));
    $age = numage2str($dprop['age']);
    $tr->appendChild($doc->createElement('td'))->appendChild($doc->createTextNode($age));
    $deventer = numage2str(time()-$dprop['deventer']);
    $tr->appendChild($doc->createElement('td'))->appendChild($doc->createTextNode($deventer));
    foreach( $showfields as $field )
      $tr->appendChild($doc->createElement('td'))->appendChild($doc->createTextNode($dprop[$field]));
    // user:
    $td = $tr->appendChild($doc->createElement('td'));
    $inp = $td->appendChild($doc->createElement('select'));
    $inp->setAttribute('oninput','location.href=\'?setroom'.'owner='.urlencode($dev).'&owner=\'+event.target.value;');
    $opt = $inp->appendChild($doc->createElement('option'));
    $opt->setAttribute('value','');
    $opt->appendChild($doc->createTextNode(' -- unclaimed -- '));
    if( $dprop['owner'] == '' )
      $opt->setAttribute('selected','');
    foreach($users as $us){
      $opt = $inp->appendChild($doc->createElement('option'));
      $opt->setAttribute('value',$us);
      $opt->appendChild($doc->createTextNode($us));
      if( $dprop['owner'] == $us )
        $opt->setAttribute('selected','');
    }
    // group:
    $td = $tr->appendChild($doc->createElement('td'));
    $inp = $td->appendChild($doc->createElement('select'));
    $inp->setAttribute('oninput','location.href=\'?setroom'.'group='.urlencode($dev).'&group=\'+event.target.value;');
    $opt = $inp->appendChild($doc->createElement('option'));
    $opt->setAttribute('value','');
    $opt->appendChild($doc->createTextNode(' -- no group -- '));
    if( $dprop['group'] == '' )
      $opt->setAttribute('selected','');
    foreach($groups as $us){
      $opt = $inp->appendChild($doc->createElement('option'));
      $opt->setAttribute('value',$us);
      $opt->appendChild($doc->createTextNode($us));
      if( $dprop['group'] == $us )
        $opt->setAttribute('selected','');
    }
    //label:
    $td = $tr->appendChild($doc->createElement('td'));
    $form = $td->appendChild($doc->createElement('form'));
    $inp = $form->appendChild($doc->createElement('input'));
    $inp->setAttribute('type','hidden');
    $inp->setAttribute('name','setroom'.'label');
    $inp->setAttribute('value',$dev);
    $inp = $form->appendChild($doc->createElement('input'));
    $inp->setAttribute('type','text');
    $inp->setAttribute('name','label');
    $inp->setAttribute('pattern','[a-zA-Z0-9-_]*');
    $inp->setAttribute('value',$dprop['label']);
    $form->appendChild($doc->createElement('button'))->appendChild($doc->createTextNode('Save'));
    // delete
    $td = $tr->appendChild($doc->createElement('td'));
    $form = $td->appendChild($doc->createElement('form'));
    $inp = $form->appendChild($doc->createElement('input'));
    $inp->setAttribute('type','checkbox');
    $inp->setAttribute('name','rmroom');
    $inp->setAttribute('value',$dev);
    $form->appendChild($doc->createElement('button'))->appendChild($doc->createTextNode('Delete'));
  }
  if( $cnt_old > 0 ){
    $form = $root->appendChild($doc->createElement('form'));
    $inp = $form->appendChild($doc->createElement('input'));
    $inp->setAttribute('type','hidden');
    $inp->setAttribute('name','rmoldrooms');
    $inp = $form->appendChild($doc->createElement('button'));
    $inp->appendChild($doc->createTextNode('Remove '.$cnt_old.' inactive unclaimed room(s)'));
  }
  echo $doc->saveHTML() . "\n";
}

function html_admin_users()
{
  $fname = '../db/ovpasswd';
  $pws = array();
  if( file_exists( $fname ) )
    $pws = json_decode( file_get_contents( $fname ), true );
  $oldusers = array();
  $fname = '../ovboxusers';
  if( file_exists( $fname ) ){
    $fcontent = file_get_contents( $fname );
    $lines = explode("\n",$fcontent);
    foreach( $lines as $line ){
      $linet = explode("#",trim($line));
      $line = $linet[0];
      if( !empty($line) ){
        $linet = explode(":",trim($line));
        if( !empty($linet[0]) ){
          if( ($linet[0]!='room') && ($linet[0]!='device') && ($linet[0]!='admin'))
            $oldusers[] = $linet[0];
        }
      }
    }
  }
  $users = list_users();
  $udb = array();
  $alldev = list_devices();
  foreach($users as $us){
    $uprop = get_properties($us,'user');
    $uprop['owneddev'] = owned_devices( $us, $alldev );
    $uprop['numdev'] = count($uprop['owneddev']);
    $uprop['numgrp'] = count(list_groups( $us ));
    $uprop['newdb'] = array_key_exists($us,$pws);
    $uprop['olddb'] = in_array($us,$oldusers);
    $uprop['uname'] = $us;
    $devage = 3600*24*365*40*2;
    foreach($uprop['owneddev'] as $d=>$dprop){
      $devage = min($devage,$dprop['age']);
    }
    $uprop['devage'] = $devage;
    $udb[$us] = $uprop;
  }
  // sort db:
  $sortprop = get_properties('list','sortkey');
  $sortkey = $sortprop['user'];
  if( $sortkey=='id' )
    ksort($udb);
  else{
    sort_comp_key( $udb, $sortprop['user'], 'uname' );
  }
  // create html:
  $doc = new DOMDocument('1.0');
  $root = $doc->createElement('div');
  $doc->appendChild($root);
  $root->appendChild($doc->createElement('a'))->setAttribute('name','users');
  $el = $root->appendChild($doc->createElement('h2'));
  $el->appendChild($doc->createTextNode('users: '));
  $s = $el->appendChild($doc->createElement('span'));
  $s->setAttribute('style','font-size: small;');
  $s->appendChild($doc->createTextNode('('.count($users).')'));
  $tab = $root->appendChild($doc->createElement('table'));
  $tr = $tab->appendChild($doc->createElement('tr'));
  //
  $headerfields = array('id','age','devage','numdev','numgrp','maingroup');
  foreach( $headerfields as $field )
    table_add_header($tr,$field,$sortkey,$doc,'user');
  $tab->appendChild($doc->createTextNode("\n"));
  $usgroups = list_groups();
  foreach($udb as $us=>$uprop){
    $tr = $tab->appendChild($doc->createElement('tr'));
    if( $uprop['agreedterms'] ){
      if( (($uprop['numdev']==0) && ($uprop['age']>60*24*3600)) ||
      ((($uprop['numdev']>0) && (($uprop['devage']>240*24*3600) && $uprop['age']>240*24*3600))) ){
        $tr->setAttribute('style','background-color: #ffaaaa;');
      }
    }else{
      if( (($uprop['numdev']==0) && ($uprop['age']>60*24*3600) && ($uprop['age']<3600*24*365*40)) ||
          ((($uprop['numdev']>0) && (($uprop['devage']>240*24*3600) && $uprop['age']>240*24*3600)) && ($uprop['age']<3600*24*365*40)) ){
        $tr->setAttribute('style','background-color: #ccaaaa;');
      }else{
        $tr->setAttribute('style','background-color: #aaaaaa;');
      }
    }
    $td = $tr->appendChild($doc->createElement('td'));
    if( !$uprop['newdb'] ){
      $td->setAttribute('style','background-color: #aaaaff;');
    }else{
      if( $uprop['olddb'] ){
        $td->setAttribute('style','background-color: #ff88aa;');
      }
    }
    $td->appendChild($doc->createTextNode($us));
    $td = $tr->appendChild($doc->createElement('td'));
    $td->appendChild($doc->createTextNode(numage2str($uprop['age'])));
    $td = $tr->appendChild($doc->createElement('td'));
    $td->appendChild($doc->createTextNode(numage2str($uprop['devage'])));
    $td = $tr->appendChild($doc->createElement('td'));
    $td->appendChild($doc->createTextNode($uprop['numdev']));
    $td = $tr->appendChild($doc->createElement('td'));
    $td->appendChild($doc->createTextNode($uprop['numgrp']));
    $td = $tr->appendChild($doc->createElement('td'));
    $form = $td->appendChild($doc->createElement('form'));
    $inp = $form->appendChild($doc->createElement('input'));
    $inp->setAttribute('type','hidden');
    $inp->setAttribute('name','moduser');
    $inp->setAttribute('value',$us);
    $inp = $form->appendChild($doc->createElement('input'));
    $inp->setAttribute('title','If checked, the user can see all public rooms, otherwise only the owned rooms.');
    $inp->setAttribute('type','checkbox');
    $inp->setAttribute('name','seesall');
    $inp->setAttribute('id','seesall');
    if( $uprop['seesall'] )
      $inp->setAttribute('checked','');
    $inp = $form->appendChild($doc->createElement('label'));
    $inp->setAttribute('for','seesall');
    $inp->appendChild($doc->createTextNode(' sees public rooms '));

    $inp = $form->appendChild($doc->createElement('select'));
    $inp->setAttribute('name','maingroup');
    $opt = $inp->appendChild($doc->createElement('option'));
    $opt->setAttribute('value','');
    $opt->appendChild($doc->createTextNode(' -- no main group -- '));
    foreach($usgroups as $grp){
      $opt = $inp->appendChild($doc->createElement('option'));
      $opt->setAttribute('value',$grp);
      $opt->appendChild($doc->createTextNode($grp));
      if( $uprop['maingroup'] == $grp )
        $opt->setAttribute('selected','');
    }
    $inp = $form->appendChild($doc->createElement('button'));
    $inp->appendChild($doc->createTextNode('Save'));
    $tab->appendChild($doc->createTextNode("\n"));
    $td = $tr->appendChild($doc->createElement('td'));
    $form = $td->appendChild($doc->createElement('form'));
    $inp = $form->appendChild($doc->createElement('input'));
    $inp->setAttribute('type','checkbox');
    $inp->setAttribute('name','adminterminateaccount');
    $inp->setAttribute('value',$us);
    $form->appendChild($doc->createElement('button'))->appendChild($doc->createTextNode('delete account and all device config'));
  }
  echo $doc->saveHTML() . "\n";
}

function html_admin_groups()
{
  cleanup_groups();
  $users = list_users();
  $grps = list_groups();
  $styles = list_styles();
  $doc = new DOMDocument('1.0');
  $root = $doc->createElement('div');
  $doc->appendChild($root);
  $root->appendChild($doc->createElement('a'))->setAttribute('name','groups');
  $el = $root->appendChild($doc->createElement('h2'));
  $el->appendChild($doc->createTextNode('groups: '));
  $s = $el->appendChild($doc->createElement('span'));
  $s->setAttribute('style','font-size: small;');
  $s->appendChild($doc->createTextNode('('.count($grps).')'));
  $form = $root->appendChild($doc->createElement('form'));
  $el = $form->appendChild($doc->createElement('input'));
  $el->setAttribute('name','addgroup');
  $el->setAttribute('type','text');
  $el->setAttribute('pattern','[a-zA-Z0-9-_]*');
  $el = $form->appendChild($doc->createElement('button'));
  $el->appendChild($doc->createTextNode('add group'));
  $tab = $root->appendChild($doc->createElement('table'));
  $tr = $tab->appendChild($doc->createElement('tr'));
  $th = $tr->appendChild($doc->createElement('th'));
  $th->appendChild($doc->createTextNode('group'));
  $th = $tr->appendChild($doc->createElement('th'));
  $th->appendChild($doc->createTextNode('users'));
  $th = $tr->appendChild($doc->createElement('th'));
  $th->appendChild($doc->createTextNode('style'));
  $tab->appendChild($doc->createTextNode("\n"));
  foreach($grps as $group){
    $gprop = get_properties( $group, 'group' );
    $grpusers = get_group_users( $group );
    sort($grpusers);
    $tr = $tab->appendChild($doc->createElement('tr'));
    $td = $tr->appendChild($doc->createElement('td'));
    $td->appendChild($doc->createTextNode($group));
    $td = $tr->appendChild($doc->createElement('td'));
    $td->setAttribute('style','line-height: 2.1em;');
    foreach( $grpusers as $us ){
      $sp = $td->appendChild($doc->createElement('span'));
      $sp->setAttribute('class','psvmember');
      $sp->appendChild($doc->createTextNode($us));
      $form = $sp->appendChild($doc->createElement('form'));
      $form->setAttribute('style','display: inline;');
      $inp = $form->appendChild($doc->createElement('input'));
      $inp->setAttribute('type','hidden');
      $inp->setAttribute('name','removeuserfromgroup');
      $inp->setAttribute('value',$group);
      $inp = $form->appendChild($doc->createElement('input'));
      $inp->setAttribute('type','hidden');
      $inp->setAttribute('name','groupuser');
      $inp->setAttribute('value',$us);
      $inp = $form->appendChild($doc->createElement('button'));
      $inp->setAttribute('class','kick');
      $inp->appendChild($doc->createTextNode('X'));
      $td->appendChild($doc->createTextNode(' '));
    }
    $inp = $td->appendChild($doc->createElement('select'));
    $inp->setAttribute('oninput','location.href=\'?addusertogroup='.urlencode($group).'&newuser=\'+event.target.value;');
    $opt = $inp->appendChild($doc->createElement('option'));
    $opt->setAttribute('value','');
    $opt->appendChild($doc->createTextNode(' -- select a user to add -- '));
    foreach($users as $us){
      if( !in_array($us,$grpusers) ){
        $opt = $inp->appendChild($doc->createElement('option'));
        $opt->setAttribute('value',$us);
        $opt->appendChild($doc->createTextNode($us));
      }
    }

    $td = $tr->appendChild($doc->createElement('td'));
    $inp = $td->appendChild($doc->createElement('select'));
    $inp->setAttribute('oninput','location.href=\'?setgrpstyle='.urlencode($group).'&grpstyle=\'+event.target.value;');
    $opt = $inp->appendChild($doc->createElement('option'));
    $opt->setAttribute('value','');
    $opt->appendChild($doc->createTextNode(' -- default style -- '));
    $usgroups = list_groups($us);
    foreach($styles as $style){
      $opt = $inp->appendChild($doc->createElement('option'));
      $opt->setAttribute('value',$style);
      $opt->appendChild($doc->createTextNode($style));
      if( $gprop['style'] == $style )
        $opt->setAttribute('selected','');
    }

    $td = $tr->appendChild($doc->createElement('td'));
    $form = $td->appendChild($doc->createElement('form'));
    $inp = $form->appendChild($doc->createElement('input'));
    $inp->setAttribute('type','hidden');
    $inp->setAttribute('name','rmgroup');
    $inp->setAttribute('value',$group);
    $inp = $form->appendChild($doc->createElement('button'));
    $inp->appendChild($doc->createTextNode('delete group'));
    $tab->appendChild($doc->createTextNode("\n"));
    $td = $tr->appendChild($doc->createElement('td'));
    $a = $td->appendChild($doc->createElement('a'));
    $a->setAttribute('href','https://box.orlandoviols.com/?grp='.grouphash( $group ));
    $a->appendChild($doc->createTextNode(grouphash( $group )));
  }
  echo $doc->saveHTML() . "\n";
}

function html_show_user( $user, $device, $devprop )
{
  $uprop = get_properties( $user, 'user' );
  $doc = new DOMDocument('1.0');
  $root = $doc->appendChild($doc->createElement('div'));
  $root->setAttribute('class','userarea');
  $p = $root->appendChild($doc->createElement('p'));
  $devspan = $p->appendChild($doc->createElement('span'));
  $devspan->setAttribute('id','deviceuser');
  $devspan->appendChild($doc->createTextNode('You are logged in as user '));
  $devspan->appendChild($doc->createElement('b'))->appendChild($doc->createTextNode($user));
  if( empty( $device ) ){
    $devspan->appendChild($doc->createTextNode(' with no device.'));
  }else{
    $class = 'psvmember';
    $state = '';
    $lastseen = '';
    $otherdev = '';
    if( $devprop['age'] < 20 ){
      $class = 'actmember';
      $state = 'active';
    }else{
      $lastseen = ', inactive since '.numage2str($devprop['age']);
      $oact = false;
      foreach( owned_devices($user) as $od=>$odp )
        if( $odp['age']<20 )
          $oact = true;
      if( $oact )
        $otherdev = ' You own active devices - please check the device selector below to access them.';
    }
    $devspan->appendChild($doc->createTextNode(' with '.$state.' device '));
    $span = $devspan->appendChild($doc->createElement('span'));
    $span->setAttribute('class',$class);
    $span->appendChild($doc->createElement('b'))->appendChild($doc->createTextNode($device.' ('.$devprop['label'].')' ));
    $devspan->appendChild($doc->createTextNode($lastseen.'.'.$otherdev));
    if( ($devprop['age'] < 20) && (($devprop['bandwidth']['tx']>0)||($devprop['bandwidth']['rx']>0)) ){
      if( $devprop['bandwidth']['tx'] >= 100000 )
        $txstr = number_format(0.000001*$devprop['bandwidth']['tx'],2).' MBps';
      else
        $txstr = number_format(0.001*$devprop['bandwidth']['tx'],2).' kBps';
      if( $devprop['bandwidth']['rx'] >= 100000 )
        $rxstr = number_format(0.000001*$devprop['bandwidth']['rx'],2).' MBps';
      else
        $rxstr = number_format(0.001*$devprop['bandwidth']['rx'],2).' kBps';
      $devspan->appendChild($doc->createTextNode(' sending: '. $txstr.', receiving: '. $rxstr));
    }
    if( ($devprop['age'] < 20) && ($devprop['cpuload']>0) ){
      $devspan->appendChild($doc->createTextNode(' CPU load: '. number_format(100*$devprop['cpuload'],1).'%'));
    }
  }
  $inf = $root->appendChild($doc->createElement('div'));
  $inf->setAttribute('id','groupinfo');
  $inf->setAttribute('class','devprop');
  $inf->setAttribute('style','display: none;');
  $form = $inf->appendChild($doc->createElement('form'));
  $form->setAttribute('method','POST');
  $el = $form->appendChild($doc->createElement('label'));
  $el->setAttribute('for','usermail');
  $el->appendChild($doc->createTextNode('e-mail address (only used for password recovery):'));
  $form->appendChild($doc->createElement('br'));
  $el = $form->appendChild($doc->createElement('input'));
  $el->setAttribute('type','email');
  $el->setAttribute('name','usermail');
  $el->setAttribute('value',$uprop['mail']);
  $el = $form->appendChild($doc->createElement('button'));
  $el->appendChild($doc->createTextNode('Save'));
  $inf->appendChild($doc->createElement('hr'));
  $el = $inf->appendChild($doc->createElement('input'));
  $el->setAttribute('value','reset password');
  $el->setAttribute('type','button');
  $el->setAttribute('onclick','location.href=\'?mypwreset\';');
  $inf->appendChild($doc->createElement('hr'));
  $el = $inf->appendChild($doc->createElement('div'));
  $el->setAttribute('class','devproptitle');
  $el->appendChild($doc->createTextNode('You are a member of these groups:'));
  foreach( list_groups($user) as $grp ){
    $p1 = $inf->appendChild($doc->createElement('div'));
    $p1->appendChild($doc->createElement('b'))->appendChild($doc->createTextNode($grp.': '));
    $grpusers = get_group_users( $grp );
    sort($grpusers);
    foreach( $grpusers as $us ){
      $p1->appendChild($doc->createTextNode($us.' '));
    }
  }
  $inf->appendChild($doc->createElement('hr'));
  // main group selector:
  $inp = $inf->appendChild($doc->createElement('label'));
  $inp->appendChild($doc->createTextNode('Primary group: '));
  $inp = $inf->appendChild($doc->createElement('select'));
  $inp->setAttribute('oninput','location.href=\'?setmaingroup=\'+event.target.value;');
  $opt = $inp->appendChild($doc->createElement('option'));
  $opt->setAttribute('value','');
  $opt->appendChild($doc->createTextNode(' -- no group -- '));
  if( $uprop['maingroup'] == '' )
    $opt->setAttribute('selected','');
  foreach(list_groups($user) as $us){
    $opt = $inp->appendChild($doc->createElement('option'));
    $opt->setAttribute('value',$us);
    $opt->appendChild($doc->createTextNode($us));
    if( $uprop['maingroup'] == $us )
      $opt->setAttribute('selected','');
  }
  $inp = $p->appendChild($doc->createElement('input'));
  // manual device claiming:
  $inf->appendChild($doc->createElement('hr'));
  $form = $inf->appendChild($doc->createElement('form'));
  $form->setAttribute('method','POST');
  $p = $form->appendChild($doc->createElement('p'));
  $p->appendChild($doc->createTextNode('If you are missing your device but you know the MAC address or device ID, you may claim it manually. To do so, please power it on and claim it within five minutes using this form.'));
  $el = $form->appendChild($doc->createElement('label'));
  $el->setAttribute('for','claimdevid');
  $el->appendChild($doc->createTextNode('MAC address / device ID:'));
  $form->appendChild($doc->createElement('br'));
  $el = $form->appendChild($doc->createElement('input'));
  $el->setAttribute('name','claimdevid');
  $el->setAttribute('id','claimdevid');
  $el = $form->appendChild($doc->createElement('button'));
  $el->appendChild($doc->createTextNode('Claim this device'));
  //
  $inp->setAttribute('type','button');
  $inp->setAttribute('class','roomsettingstoggle uibutton');
  $inp->setAttribute('onclick','toggledisplay(\'groupinfo\',\'user profile\');');
  $inp->setAttribute('value','show user profile');
  echo $doc->saveHTML() . "\n";
}

function add_expert_div( &$el, &$doc )
{
  $div = $el->appendChild( $doc->createElement('div') );
  //$div->setAttribute('id','expert');
  $div->setAttribute('class','expert');
  $div->setAttribute('style','display: none;');
  return $div;
}

function add_input_element( &$parent, &$doc, &$devprop, $prop, $type, $label, $endbr = true )
{
  $el = $parent->appendChild($doc->createElement('label'));
  $el->setAttribute('for',$prop);
  $el->appendChild($doc->createTextNode($label));
  $parent->appendChild($doc->createElement('br'));
  $el = $parent->appendChild($doc->createElement('input'));
  $el->setAttribute('id',$prop);
  $el->setAttribute('name',$prop);
  $el->setAttribute('type',$type);
  $el->setAttribute('value',$devprop[$prop]);
  if( $endbr )
    $parent->appendChild($doc->createElement('br'));
  return $el;
}

function html_show_device( $user, $device, $devprop )
{
  html_device_selector( $user, $device );
  $doc = new DOMDocument('1.0');
  $root = $doc->appendChild($doc->createElement('div'));
  $devset = $root->appendChild($doc->createElement('div'));
  $devset->setAttribute('class','devprop');
  $devset->setAttribute('id','devsettings');
  $devset->setAttribute('style','display: none;');
  // preset list:
  $el = $devset->appendChild($doc->createElement('div'));
  $el->setAttribute('class','devproptitle');
  $el->appendChild($doc->createTextNode('Device setting presets:'));
  $presets = get_properties( $device, 'devpresets' );
  unset($presets['now']);
  $el = $devset->appendChild($doc->createElement('div'));
  $el->setAttribute('class','presetarea');
  if( !empty($presets) ){
    $presets = array_keys($presets);
    foreach( $presets as $preset ){
      $xclass = '';
      if( $preset == $devprop['preset'] )
        $xclass = ' presetact';
      $span = $el->appendChild($doc->createElement('span'));
      $span->setAttribute('class','presetspan'.$xclass);
      $inp = $span->appendChild($doc->createElement('input'));
      $inp->setAttribute('class','presetload'.$xclass);
      $inp->setAttribute('type','button');
      $inp->setAttribute('value',$preset);
      $inp->setAttribute('onclick','load_preset(this.value);');
      $inp = $span->appendChild($doc->createElement('input'));
      $inp->setAttribute('class','presetrm');
      $inp->setAttribute('type','button');
      $inp->setAttribute('value','X');
      $inp->setAttribute('name',$preset);
      $inp->setAttribute('onclick','rm_preset(this.name);');
    }
  }
  // settings presets:
  $span = $el->appendChild($doc->createElement('span'));
  $span->setAttribute('class','presetspan');
  $inp = $span->appendChild($doc->createElement('input'));
  $inp->setAttribute('id','savepresetname');
  //$inp->setAttribute('class','presetspan');
  $inp->setAttribute('placeholder','Save current settings as preset');
  $inp->setAttribute('onchange','create_preset();');
  //$inp = $span->appendChild($doc->createElement('input'));
  //$inp->setAttribute('type','button');
  //$inp->setAttribute('value','Create new');
  //$inp->setAttribute('onclick','create_preset();');
  $devset->appendChild($doc->createElement('hr'));
  // device settings:
  $form = $devset->appendChild($doc->createElement('form'));
  $form->setAttribute('method','POST');
  $inp = $form->appendChild($doc->createElement('input'));
  $inp->setAttribute('type','button');
  $inp->setAttribute('class','roomsettingstoggle uibutton');
  $inp->setAttribute('onclick','toggledisplayclass(\'expert\',\'expert settings\');');
  $inp->setAttribute('value','show expert settings');
  $el = $form->appendChild($doc->createElement('div'));
  $el->setAttribute('class','devproptitle');
  $el->appendChild($doc->createTextNode('Device settings:'));
  $span = $el->appendChild($doc->createElement('span'));
  $span->setAttribute('class','valuechanged important');
  // device properties:
  // label:
  $el = $form->appendChild($doc->createElement('label'));
  $el->setAttribute('for','label');
  $el->appendChild($doc->createTextNode('device label (appears in rooms and the mixer of the others): '));
  $form->appendChild($doc->createElement('br'));
  $el = $form->appendChild($doc->createElement('input'));
  $el->setAttribute('id','label');
  $el->setAttribute('name','label');
  $el->setAttribute('type','text');
  $el->setAttribute('pattern','[a-zA-Z0-9-_]*');
  $el->setAttribute('value',$devprop['label']);
  $el->setAttribute('oninput','dispvaluechanged("valuechanged");');
  $form->appendChild($doc->createElement('br'));
  $form->appendChild($doc->createElement('hr'));
  // sound card settings:
  $div = $form->appendChild($doc->createElement('div'));
  $el = $div->appendChild($doc->createElement('div'));
  $div->setAttribute('id','sndcardsettings');
  $div->appendChild($doc->createTextNode('Sound card:'));
  $div->appendChild($doc->createElement('br'));
  // jack device:
  $el = $div->appendChild($doc->createElement('select'));
  $el->setAttribute('id','jackdevice');
  $el->setAttribute('name','jackdevice');
  $el->setAttribute('oninput','dispvaluechanged("valuechanged");');
  //
  $alsadevs = array('highest'=>'use highest device number','manual'=>'jack is started manually','hw:1'=>'device 1 (typically first USB device)');
  if( is_array($devprop['alsadevs']) )
    $alsadevs = array_merge( $alsadevs, $devprop['alsadevs']);
  foreach( $alsadevs as $adev=>$desc ){
    $opt = $el->appendChild($doc->createElement('option'));
    $opt->setAttribute('value',$adev);
    $opt->appendChild($doc->createTextNode($desc . ' ('.$adev.')'));
    if( $devprop['jackdevice'] == $adev )
      $opt->setAttribute('selected','');
  }
  // use plughw device:
  $el = $div->appendChild($doc->createElement('input'));
  $el->setAttribute('type','checkbox');
  $el->setAttribute('name','jackplugdev');
  $el->setAttribute('id','jackplugdev');
  $el->setAttribute('value','jackplugdev');
  $el->setAttribute('title','activate to use sampling rates not supported by hardware (reduces quality)');
  $el->setAttribute('oninput','dispvaluechanged("valuechanged");');
  if( $devprop['jackplugdev'] )
    $el->setAttribute('checked','');
  $el = $div->appendChild($doc->createElement('label'));
  $el->setAttribute('title','activate to use sampling rates not supported by hardware (reduces quality)');
  $el->setAttribute('for','jackplugdev');
  $el->appendChild($doc->createTextNode('use plugin device layer'));
  $div->appendChild($doc->createElement('br'));
  // end of device.
  $el = add_input_element( $div, $doc, $devprop, 'jackrate', 'number','Sampling rate in Hz: ',false);
  $el->setAttribute('oninput','dispvaluechanged("valuechanged");');
  $el = $div->appendChild($doc->createElement('select'));
  $el->setAttribute('oninput','dispvaluechanged("valuechanged");');
  $el->setAttribute('onchange','update_jack_rate( this.value );');
  $opts = array('16000','22050','24000','32000','44100','48000');
  foreach($opts as $o){
    $opt = $el->appendChild($doc->createElement('option'));
    $opt->setAttribute('value',$o);
    if( $o == $devprop['jackrate'] )
      $opt->setAttribute('selected','');
    $opt->appendChild($doc->createTextNode($o));
  }
  $div->appendChild($doc->createElement('br'));
  $el = add_input_element( $div, $doc, $devprop, 'jackperiod', 'number','Period size in samples (typically 2ms, i.e. 96 for 48000 Hz Sampling rate): ');
  $el->setAttribute('oninput','dispvaluechanged("valuechanged");');
  $divex = add_expert_div($form,$doc);
  $el = add_input_element( $divex, $doc, $devprop, 'jackbuffers', 'number','Number of buffers (typically 2): ');
  $el->setAttribute('oninput','dispvaluechanged("valuechanged");');
  $el->setAttribute('min',2);
  if( version_compare("ovclient-0.5.11-8cc47fd",$devprop['version'])<0 ){
    $div->appendChild($doc->createElement('br'));
    $el = $div->appendChild($doc->createElement('input'));
    $el->setAttribute('value','restart audio system');
    $el->setAttribute('type','button');
    $el->setAttribute('onclick','location.href=\'?jackrestart='.urlencode($device).'\';');
  }
  $form->appendChild($doc->createElement('hr'));
  // Audio settings:
  $el = $form->appendChild($doc->createElement('div'));
  $el->setAttribute('class','devproptitle');
  $el->appendChild($doc->createTextNode('Audio settings:'));
  $span = $el->appendChild($doc->createElement('span'));
  $span->setAttribute('class','valuechanged important');
  // input port:
  create_inputportcfg( $doc, $form, $devprop );
  // output port:
  $div = add_expert_div($form,$doc);
  $el = $div->appendChild($doc->createElement('label'));
  $el->setAttribute('for','outputport');
  $el->appendChild($doc->createTextNode('output ports (to which your headphones are connected): '));
  $div->appendChild($doc->createElement('br'));
  $el = $div->appendChild($doc->createElement('input'));
  $el->setAttribute('oninput','dispvaluechanged("valuechanged");');
  $el->setAttribute('id','outputport1');
  $el->setAttribute('name','outputport1');
  $el->setAttribute('type','text');
  $el->setAttribute('value',$devprop['outputport1']);
  $el = $div->appendChild($doc->createElement('input'));
  $el->setAttribute('oninput','dispvaluechanged("valuechanged");');
  $el->setAttribute('id','outputport2');
  $el->setAttribute('name','outputport2');
  $el->setAttribute('type','text');
  $el->setAttribute('value',$devprop['outputport2']);
  $div->appendChild($doc->createElement('br'));
  // extra ports:
  $el = $div->appendChild($doc->createElement('label'));
  $el->setAttribute('for','xport');
  $el->appendChild($doc->createTextNode('extra ports (json expression, e.g., {"Giso:out_1":"ardour:Giso/in"}): '));
  $div->appendChild($doc->createElement('br'));
  $el = $div->appendChild($doc->createElement('input'));
  $el->setAttribute('oninput','dispvaluechanged("valuechanged");');
  $el->setAttribute('size','40');
  $el->setAttribute('id','xport');
  $el->setAttribute('name','xport');
  $el->setAttribute('type','text');
  $el->setAttribute('value',$devprop['xport']);
  $div->appendChild($doc->createElement('br'));
  // receiver type:
  $el = $div->appendChild($doc->createElement('label'));
  $el->setAttribute('for','rectype');
  $el->appendChild($doc->createTextNode('receiver type: '));
  $el = $div->appendChild($doc->createElement('select'));
  $el->setAttribute('oninput','dispvaluechanged("valuechanged");');
  $el->setAttribute('id','rectype');
  $el->setAttribute('name','rectype');
  $recdesc = array('ortf'=>'Commonly used stereo microphone technique','hrtf'=>'Binaural Head Related Transfer Function simulation','itu51'=>'ITU 5.1 rendering, channel order L,R,C,LFE,Ls,Rs','omni'=>'mono');
  foreach( $recdesc as $rectype=>$desc ){
    $opt = $el->appendChild($doc->createElement('option'));
    $opt->setAttribute('value',$rectype);
    if( $devprop['rectype'] == $rectype )
      $opt->setAttribute('selected','');
    $opt->appendChild($doc->createTextNode($rectype.': '.$desc));
  }
  $div->appendChild($doc->createElement('br'));
  // playback gain:
  $el = $form->appendChild($doc->createElement('label'));
  $el->setAttribute('for','playbackgain');
  $el->appendChild($doc->createTextNode('playback gain in dB (equivalent to changing the input gain): '));
  $form->appendChild($doc->createElement('br'));
  $el = $form->appendChild($doc->createElement('input'));
  $el->setAttribute('oninput','dispvaluechanged("valuechanged");');
  $el->setAttribute('id','playbackgain');
  $el->setAttribute('name','playbackgain');
  $el->setAttribute('type','number');
  $el->setAttribute('min','-20');
  $el->setAttribute('max','20');
  $el->setAttribute('step','0.1');
  $el->setAttribute('value',$devprop['playbackgain']);
  $form->appendChild($doc->createElement('br'));
  // master gain:
  $el = $form->appendChild($doc->createElement('label'));
  $el->setAttribute('for','playbackgain');
  $el->appendChild($doc->createTextNode('master gain in dB (equivalent to changing the headphone gain): '));
  $form->appendChild($doc->createElement('br'));
  $el = $form->appendChild($doc->createElement('input'));
  $el->setAttribute('oninput','dispvaluechanged("valuechanged");');
  $el->setAttribute('id','mastergain');
  $el->setAttribute('name','mastergain');
  $el->setAttribute('type','number');
  $el->setAttribute('min','-20');
  $el->setAttribute('max','20');
  $el->setAttribute('step','0.1');
  $el->setAttribute('value',$devprop['mastergain']);
  $form->appendChild($doc->createElement('br'));
  // ego monitor:
  $el = $form->appendChild($doc->createElement('label'));
  $el->setAttribute('for','egogain');
  $el->appendChild($doc->createTextNode('ego monitor gain in dB (how much of your own microphone is added to your headphone): '));
  $form->appendChild($doc->createElement('br'));
  $el = $form->appendChild($doc->createElement('input'));
  $el->setAttribute('oninput','dispvaluechanged("valuechanged");');
  $el->setAttribute('id','egogain');
  $el->setAttribute('name','egogain');
  $el->setAttribute('type','number');
  $el->setAttribute('min','-30');
  $el->setAttribute('max','10');
  $el->setAttribute('step','0.1');
  $el->setAttribute('value',$devprop['egogain']);
  $lab = $form->appendChild($doc->createElement('label'));
  $lab->setAttribute('class','switch');
  $el = $lab->appendChild($doc->createElement('input'));
  $el->setAttribute('oninput','dispvaluechanged("valuechanged");');
  $el->setAttribute('id','selfmonitor');
  $el->setAttribute('name','selfmonitor');
  $el->setAttribute('type','checkbox');
  if( $devprop['selfmonitor'] )
    $el->setAttribute('checked','');
  $el = $lab->appendChild($doc->createElement('span'));
  $el->setAttribute('class','slider round');
  $el->appendChild($doc->createTextNode(''));
  $form->appendChild($doc->createElement('br'));
  //
  $div = add_expert_div($form,$doc);
  // ego monitor delay:
  $el = $div->appendChild($doc->createElement('label'));
  $el->setAttribute('for','selfmonitordelay');
  $el->appendChild($doc->createTextNode('self monitor delay in milliseconds: '));
  $div->appendChild($doc->createElement('br'));
  $el = $div->appendChild($doc->createElement('input'));
  $el->setAttribute('oninput','dispvaluechanged("valuechanged");');
  $el->setAttribute('id','selfmonitordelay');
  $el->setAttribute('name','selfmonitordelay');
  $el->setAttribute('type','number');
  $el->setAttribute('min','0');
  $el->setAttribute('max','1000');
  $el->setAttribute('step','1');
  $el->setAttribute('value',$devprop['selfmonitordelay']);
  $div->appendChild($doc->createElement('br'));
  // reverb:
  $el = $div->appendChild($doc->createElement('input'));
  $el->setAttribute('oninput','dispvaluechanged("valuechanged");');
  $el->setAttribute('name','reverb');
  $el->setAttribute('id','reverb');
  $el->setAttribute('type','checkbox');
  if( $devprop['reverb'] )
    $el->setAttribute('checked','');
  $el = $div->appendChild($doc->createElement('label'));
  $el->setAttribute('for','reverb');
  $el->appendChild($doc->createTextNode('render reverb '));
  $div->appendChild($doc->createElement('br'));
  // ism
    $el = $div->appendChild($doc->createElement('input'));
  $el->setAttribute('oninput','dispvaluechanged("valuechanged");');
  $el->setAttribute('name','renderism');
  $el->setAttribute('id','renderism');
  $el->setAttribute('type','checkbox');
  if( $devprop['renderism'] )
    $el->setAttribute('checked','');
  $el = $div->appendChild($doc->createElement('label'));
  $el->setAttribute('for','renderism');
  $el->appendChild($doc->createTextNode('render shoebox ISM '));
  $div->appendChild($doc->createElement('br'));
  // reverb gain:
  $el = $div->appendChild($doc->createElement('label'));
  $el->setAttribute('for','devrvbgain');
  $el->appendChild($doc->createTextNode('extra reverb gain in dB: '));
  $div->appendChild($doc->createElement('br'));
  $el = $div->appendChild($doc->createElement('input'));
  $el->setAttribute('oninput','dispvaluechanged("valuechanged");');
  $el->setAttribute('id','devrvbgain');
  $el->setAttribute('name','rvbgain');
  $el->setAttribute('type','number');
  $el->setAttribute('min','-20');
  $el->setAttribute('max','20');
  $el->setAttribute('step','0.1');
  $el->setAttribute('value',$devprop['rvbgain']);
  $div->appendChild($doc->createElement('br'));
  // raw mode:
  $el = $div->appendChild($doc->createElement('input'));
  $el->setAttribute('oninput','dispvaluechanged("valuechanged");');
  $el->setAttribute('name','rawmode');
  $el->setAttribute('id','rawmode');
  $el->setAttribute('type','checkbox');
  if( $devprop['rawmode'] )
    $el->setAttribute('checked','');
  $el = $div->appendChild($doc->createElement('label'));
  $el->setAttribute('for','rawmode');
  $el->appendChild($doc->createTextNode('raw mode - no virtual acoustics '));
  // head tracking:
  $div = add_expert_div($form,$doc);
  $el = $div->appendChild($doc->createElement('div'));
  $el->setAttribute('class','devproptitle');
  $el->appendChild($doc->createTextNode('Head tracking:'));
  $el = $div->appendChild($doc->createElement('input'));
  $el->setAttribute('oninput','dispvaluechanged("valuechanged");');
  $el->setAttribute('name','headtracking');
  $el->setAttribute('id','headtracking');
  $el->setAttribute('type','checkbox');
  if( $devprop['headtracking'] )
    $el->setAttribute('checked','');
  $el = $div->appendChild($doc->createElement('label'));
  $el->setAttribute('for','headtracking');
  $el->appendChild($doc->createTextNode('load headtracking module '));
  $div->appendChild($doc->createElement('br'));
  // apply headtracking:
  $el = $div->appendChild($doc->createElement('input'));
  $el->setAttribute('oninput','dispvaluechanged("valuechanged");');
  $el->setAttribute('name','headtrackingrot');
  $el->setAttribute('id','headtrackingrot');
  $el->setAttribute('type','checkbox');
  if( $devprop['headtrackingrot'] )
    $el->setAttribute('checked','');
  $el = $div->appendChild($doc->createElement('label'));
  $el->setAttribute('for','headtrackingrot');
  $el->appendChild($doc->createTextNode('apply rotation to receiver'));
  $div->appendChild($doc->createElement('br'));
  //
  $el = $div->appendChild($doc->createElement('input'));
  $el->setAttribute('oninput','dispvaluechanged("valuechanged");');
  $el->setAttribute('name','headtrackingrotsrc');
  $el->setAttribute('id','headtrackingrotsrc');
  $el->setAttribute('type','checkbox');
  if( $devprop['headtrackingrotsrc'] )
    $el->setAttribute('checked','');
  $el = $div->appendChild($doc->createElement('label'));
  $el->setAttribute('for','headtrackingrotsrc');
  $el->appendChild($doc->createTextNode('apply rotation to source'));
  $div->appendChild($doc->createElement('br'));
  // tau auto-reference
  $el = $div->appendChild($doc->createElement('label'));
  $el->appendChild($doc->createTextNode('auto-referencing time constant in seconds: '));
  $div->appendChild($doc->createElement('br'));
  $el = $div->appendChild($doc->createElement('input'));
  $el->setAttribute('oninput','dispvaluechanged("valuechanged");');
  $el->setAttribute('value',round($devprop['headtrackingtauref'],1));
  $el->setAttribute('id','headtrackingtauref');
  $el->setAttribute('name','headtrackingtauref');
  $el->setAttribute('type','number');
  $el->setAttribute('min','0');
  $el->setAttribute('max','500');
  $el->setAttribute('step','0.1');
  $div->appendChild($doc->createElement('br'));
  //
  $el = $div->appendChild($doc->createElement('label'));
  $el->appendChild($doc->createTextNode('data logging port: '));
  $div->appendChild($doc->createElement('br'));
  $el = $div->appendChild($doc->createElement('input'));
  $el->setAttribute('oninput','dispvaluechanged("valuechanged");');
  $el->setAttribute('value',$devprop['headtrackingport']);
  $el->setAttribute('id','headtrackingport');
  $el->setAttribute('name','headtrackingport');
  $el->setAttribute('type','number');
  $el->setAttribute('min','0');
  $el->setAttribute('max','65535');
  $el->setAttribute('step','1');
  // network settings:
  $form->appendChild($doc->createElement('hr'));
  $el = $form->appendChild($doc->createElement('div'));
  $el->setAttribute('class','devproptitle');
  $el->appendChild($doc->createTextNode('Network settings:'));
  $span = $el->appendChild($doc->createElement('span'));
  $span->setAttribute('class','valuechanged important');
  // jitter (send):
  $el = $form->appendChild($doc->createElement('label'));
  $el->setAttribute('for','jittersend');
  $el->appendChild($doc->createTextNode('sender jitter (affects buffer length of others): '));
  $form->appendChild($doc->createElement('br'));
  $el = $form->appendChild($doc->createElement('input'));
  $el->setAttribute('oninput','dispvaluechanged("valuechanged");');
  $el->setAttribute('id','jittersend');
  $el->setAttribute('name','jittersend');
  $el->setAttribute('type','number');
  $el->setAttribute('min','1');
  $el->setAttribute('max','250');
  $el->setAttribute('step','1');
  $el->setAttribute('value',$devprop['jittersend']);
  $form->appendChild($doc->createElement('br'));
  // jitter (receive):
  $el = $form->appendChild($doc->createElement('label'));
  $el->setAttribute('for','jitterreceive');
  $el->appendChild($doc->createTextNode('receiver jitter (affects your own buffer length): '));
  $form->appendChild($doc->createElement('br'));
  $el = $form->appendChild($doc->createElement('input'));
  $el->setAttribute('oninput','dispvaluechanged("valuechanged");');
  $el->setAttribute('id','jitterreceive');
  $el->setAttribute('name','jitterreceive');
  $el->setAttribute('type','number');
  $el->setAttribute('min','1');
  $el->setAttribute('max','250');
  $el->setAttribute('step','1');
  $el->setAttribute('value',$devprop['jitterreceive']);
  $form->appendChild($doc->createElement('br'));
  // peer-to-peer:
  $el = $form->appendChild($doc->createElement('input'));
  $el->setAttribute('oninput','dispvaluechanged("valuechanged");');
  $el->setAttribute('name','peer2peer');
  $el->setAttribute('id','peer2peer');
  $el->setAttribute('type','checkbox');
  if( $devprop['peer2peer'] )
    $el->setAttribute('checked','');
  $el = $form->appendChild($doc->createElement('label'));
  $el->setAttribute('for','peer2peer');
  $el->appendChild($doc->createTextNode('peer-to-peer mode '));
  $form->appendChild($doc->createElement('br'));
  // extra destinations:
  $div = add_expert_div($form, $doc );
  $el = $div->appendChild($doc->createElement('input'));
  $el->setAttribute('oninput','dispvaluechanged("valuechanged");');
  $el->setAttribute('name','sendlocal');
  $el->setAttribute('id','sendlocal');
  $el->setAttribute('type','checkbox');
  if( $devprop['sendlocal'] )
    $el->setAttribute('checked','');
  $el = $div->appendChild($doc->createElement('label'));
  $el->setAttribute('for','sendlocal');
  $el->appendChild($doc->createTextNode('send to local IP address if in same network '));
  $div->appendChild($doc->createElement('br'));
  $el = $div->appendChild($doc->createElement('label'));
  $el->setAttribute('for','secrec');
  $el->appendChild($doc->createTextNode('additional local receiver delay for secondary receiver (0 for no secondary receiver): '));
  $div->appendChild($doc->createElement('br'));
  $el = $div->appendChild($doc->createElement('input'));
  $el->setAttribute('oninput','dispvaluechanged("valuechanged");');
  $el->setAttribute('id','secrec');
  $el->setAttribute('name','secrec');
  $el->setAttribute('type','number');
  $el->setAttribute('min','0');
  $el->setAttribute('max','100');
  $el->setAttribute('step','1');
  $el->setAttribute('value',$devprop['secrec']);
  $div->appendChild($doc->createElement('br'));
  $el = $div->appendChild($doc->createElement('label'));
  $el->setAttribute('for','xrecport');
  $el->appendChild($doc->createTextNode('additional UDP ports forwarded to other peers (space separated list): '));
  $div->appendChild($doc->createElement('br'));
  $el = $div->appendChild($doc->createElement('input'));
  $el->setAttribute('oninput','dispvaluechanged("valuechanged");');
  $el->setAttribute('id','xrecport');
  $el->setAttribute('name','xrecport');
  $el->setAttribute('type','text');
  $el->setAttribute('pattern','[0-9 ]*');
  $el->setAttribute('value',implode(" ",$devprop['xrecport']));
  $div->appendChild($doc->createElement('br'));
  // proxy settings
  $el = $div->appendChild($doc->createElement('input'));
  $el->setAttribute('oninput','rest_setval(\'isproxy\',this.checked);');
  $el->setAttribute('type','checkbox');
  $el->setAttribute('id','devisproxy');
  if( $devprop['isproxy'] )
    $el->setAttribute('checked','');
  $div->appendChild($doc->createTextNode('act as proxy'));
  $div->appendChild($doc->createElement('br'));
  $el = $div->appendChild($doc->createElement('input'));
  $el->setAttribute('oninput','rest_setval(\'useproxy\',this.checked);');
  $el->setAttribute('type','checkbox');
  $el->setAttribute('id','devuseproxy');
  if( $devprop['useproxy'] )
    $el->setAttribute('checked','');
  $div->appendChild($doc->createTextNode('use a proxy'));
  $div->appendChild($doc->createElement('br'));
  // frontend:
  $div->appendChild($doc->createElement('hr'));
  $el = $div->appendChild($doc->createElement('label'));
  $el->appendChild($doc->createTextNode('If required change frontend: '));
  $el = $div->appendChild($doc->createElement('select'));
  $el->setAttribute('oninput','dispvaluechanged("valuechanged");');
  $el->setAttribute('name','jsfrontendconfig');
  $opt = $el->appendChild($doc->createElement('option'));
  $opt->setAttribute('value','{}');
  $opt->appendChild($doc->createTextNode('-- switch frontend --'));
  $opt = $el->appendChild($doc->createElement('option'));
  $opt->setAttribute('value','{"url":"http://oldbox.orlandoviols.com/","protocol":"ov","ui":"https://box.orlandoviols.com/"}');
  $opt->appendChild($doc->createTextNode('box.orlandoviols.com'));
  $opt = $el->appendChild($doc->createElement('option'));
  $opt->setAttribute('value','{"url":"http://dev.ovbox.de/","protocol":"ov","ui":"https://ovbox.de/"}');
  $opt->appendChild($doc->createTextNode('ovbox.de'));
  $div->appendChild($doc->createElement('br'));
  $div->appendChild($doc->createElement('b'))->appendChild($doc->createTextNode('Warning: '));;
  $div->appendChild($doc->createTextNode('By selecting a frontend you may lock your device. In that case please delete the file "ov-client.cfg" on the boot partition of the SD card.'));
  $div->appendChild($doc->createElement('br'));
  // developer version:
  $inp = $div->appendChild($doc->createElement('input'));
  $inp->setAttribute('type','button');
  $inp->setAttribute('onclick','location.href=\'?usedevversion='.$device.'\';');
  $inp->setAttribute('value','switch to development version');
  $inp->setAttribute('class','uibutton');
  // save:
  $form->appendChild($doc->createElement('hr'));
  $el = $form->appendChild($doc->createElement('button'));
  $el->setAttribute('class','uibutton');
  $el->setAttribute('id','devsettingssave');
  $el->appendChild($doc->createTextNode('Save'));;
  $el = $form->appendChild($doc->createElement('input'));
  $el->setAttribute('name','setdevprop');
  $el->setAttribute('value',$device);
  $el->setAttribute('type','hidden');
  $inp = $form->appendChild($doc->createElement('input'));
  $inp->setAttribute('type','button');
  $inp->setAttribute('onclick','location.href=\'?\';');
  $inp->setAttribute('value','Cancel');
  $inp->setAttribute('class','uibutton');
  $inp = $form->appendChild($doc->createElement('input'));
  $inp->setAttribute('type','button');
  $inp->setAttribute('onclick','location.href=\'?devreset\';');
  $inp->setAttribute('value','Reset settings');
  $inp->setAttribute('class','uibutton');
  // firmware update:
  $devset->appendChild($doc->createElement('hr'));
  $fname = '../db/clver';
  $clver = '';
  if( file_exists( $fname ) )
    $clver = trim(file_get_contents( $fname ));
  if( !empty($devprop['version']) ){
    $el = $devset->appendChild($doc->createElement('div'));
    $el->setAttribute('class','devproptitle');
    $el->appendChild($doc->createTextNode('Device version:'));
    $devset->appendChild($doc->createTextNode($devprop['version']));
    if( version_compare($clver,$devprop['version'])==0 )
      $devset->appendChild($doc->createTextNode(' - your device is up to date.'));
  }
  if(  !empty($clver) && (substr($devprop['version'],0,9)=='ovclient-') &&
       (version_compare($clver,$devprop['version'])==1)){
    $div = $devset->appendChild($doc->createElement('div'));
    $div->setAttribute('class','devprop');
    $el = $div->appendChild($doc->createElement('div'));
    $el->setAttribute('class','devproptitle');
    $el->appendChild($doc->createTextNode('Firmware update:'));
    $div->appendChild($doc->createTextNode('Your device is running version '.$devprop['version'].', the latest version is '.$clver.'. '));
    if( (version_compare($devprop['version'],'ovclient-0.4.41')==1) ){
      $div->appendChild($doc->createTextNode('Before starting the firmware update, please connect your device with a
network cable. Once started, do not disconnect your device from the
power supply or network until the firmware update is completed. The
update may take up to 30 minutes.'));
      $div->appendChild($doc->createElement('br'));
      if($devprop['age']>=20){
        $div->appendChild($doc->createTextNode('Please start your device to update the firmware.'));
        $div->appendChild($doc->createElement('br'));
      }
      $a = $div->appendChild($doc->createElement('a'));
      $a->setAttribute('target','blank');
      $a->setAttribute('href','https://raw.githubusercontent.com/gisogrimm/ov-client/master/changelog');
      $a->appendChild($doc->createTextNode('recent changes'));
      $div->appendChild($doc->createElement('br'));
      $inp = $div->appendChild($doc->createElement('input'));
      $inp->setAttribute('type','button');
      $inp->setAttribute('onclick','location.href=\'?firmwareupdate='.$device.'\';');
      $inp->setAttribute('value','update now');
      $inp->setAttribute('class','uibutton');
    }else{
      $div->appendChild($doc->createTextNode('To update the firmware, please follow the instructions '));
      $a = $div->appendChild($doc->createElement('a'));
      $a->setAttribute('target','blank');
      $a->setAttribute('href','https://github.com/gisogrimm/ovbox/wiki/Installation');
      $a->appendChild($doc->createTextNode('here.'));
    }
  }
  // transfer ownership:
  $devset->appendChild($doc->createElement('hr'));
  $el = $devset->appendChild($doc->createElement('div'));
  $el->setAttribute('class','devproptitle');
  $el->appendChild($doc->createTextNode('Device ownership:'));
  //
  $form = $devset->appendChild($doc->createElement('form'));
  $form->setAttribute('method','POST');
  $el = $form->appendChild($doc->createElement('label'));
  $el->appendChild($doc->createTextNode('New owner:'));
  $form->appendChild($doc->createElement('br'));
  $el = $form->appendChild($doc->createElement('input'));
  $el->setAttribute('type','text');
  $el->setAttribute('name','newowner');
  $el = $form->appendChild($doc->createElement('input'));
  $el->setAttribute('name','transferownership');
  $el->setAttribute('value',$device);
  $el->setAttribute('type','hidden');
  $el = $form->appendChild($doc->createElement('button'));
  $el->setAttribute('class','uibutton');
  $el->appendChild($doc->createTextNode('transfer ownership'));;
  // not my device:
  $inp = $form->appendChild($doc->createElement('input'));
  $inp->setAttribute('type','button');
  $inp->setAttribute('class','roomsettingstoggle uibutton');
  $inp->setAttribute('onclick','location.href=\'?unclaim\';');
  $inp->setAttribute('value','not my device');

  // metronome:
  if( version_compare("ovclient-0.5.42-656035c",$devprop['version'])<0 ){
    $div = $root->appendChild($doc->createElement('div'));
    $div->setAttribute('id','metronome');
    $div->setAttribute('style','display: none;');
    // use IP address for mixer if possible:
    $div->setAttribute('class','mixer');
    // device is active and we know the host name:
    $div->setAttribute('style','display: block;');
    $inpactive = $div->appendChild($doc->createElement('input'));
    $inpactive->setAttribute('type','checkbox');
    if( $devprop['metroactive'] )
      $inpactive->setAttribute('checked','');
    $inpactive->setAttribute('onchange','setmetro(\'active\',this.checked);');
    $div->appendChild($doc->createTextNode(' metronome '));
    $div->appendChild($doc->createElement('br'));
    $inpbpm = $div->appendChild($doc->createElement('input'));
    $inpbpm->setAttribute('type','number');
    $inpbpm->setAttribute('min','1');
    $inpbpm->setAttribute('step','1');
    $inpbpm->setAttribute('max','400');
    $inpbpm->setAttribute('size','5');
    $inpbpm->setAttribute('value',$devprop['metrobpm']);
    $inpbpm->setAttribute('onchange','setmetro(\'bpm\',this.value);');
    $div->appendChild($doc->createTextNode(' Beats per Minute'));
    $div->appendChild($doc->createElement('br'));
    $inpbpb = $div->appendChild($doc->createElement('input'));
    $inpbpb->setAttribute('type','number');
    $inpbpb->setAttribute('min','1');
    $inpbpb->setAttribute('step','1');
    $inpbpb->setAttribute('max','20');
    $inpbpb->setAttribute('size','5');
    $inpbpb->setAttribute('value',$devprop['metrobpb']);
    $inpbpb->setAttribute('onchange','setmetro(\'bpb\',this.value);');
    $div->appendChild($doc->createTextNode(' Beats per Bar'));
    $div->appendChild($doc->createElement('br'));
    $inplevel = $div->appendChild($doc->createElement('input'));
    $inplevel->setAttribute('type','number');
    $inplevel->setAttribute('min','0');
    $inplevel->setAttribute('step','1');
    $inplevel->setAttribute('max','90');
    $inplevel->setAttribute('size','5');
    $inplevel->setAttribute('value',$devprop['metrolevel']);
    $inplevel->setAttribute('onchange','setmetro(\'level\',this.value);');
    $div->appendChild($doc->createTextNode(' level/dB'));
    $div->appendChild($doc->createElement('br'));
    $inpdelay = $div->appendChild($doc->createElement('input'));
    $inpdelay->setAttribute('type','number');
    $inpdelay->setAttribute('min','0');
    $inpdelay->setAttribute('step','1');
    $inpdelay->setAttribute('max','100');
    $inpdelay->setAttribute('size','5');
    $inpdelay->setAttribute('value',$devprop['metrodelay']);
    $inpdelay->setAttribute('onchange','setmetro(\'delay\',this.value);');
    $div->appendChild($doc->createTextNode(' local delay/ms'));
  }
  // web mixer:
  $div = $root->appendChild($doc->createElement('div'));
  $div->setAttribute('id','webmixerlink');
  $div->setAttribute('style','display: none;');
  if( $devprop['age']<20 ){
    $shown = false;
    foreach( array($devprop['localip'],$devprop['host']) as $mixer ){
      if( !(empty($mixer)||$shown) ){
        // device is active and we know the host name:
        $div->setAttribute('style','display: block;');
        $a = $div->appendChild($doc->createElement('a'));
        $a->setAttribute('href','http://'.$mixer.':8080/');
        $a->setAttribute('target','_blank');
        $a->setAttribute('rel','noopener noreferrer');
        $a->setAttribute('class','mixer');
        $div->setAttribute('class','mixer');
        $a->appendChild($doc->createTextNode('open mixer'));
        $div->appendChild($doc->createTextNode(' (works only when your browser is in the same network as your device)'));
        $shown = true;
      }
    }
  }
  echo $doc->saveHTML() . "\n";
}

function create_inputportcfg( &$doc, &$form, $devprop )
{
  // store input channel configuration:
  $el = $form->appendChild($doc->createElement('input'));
  $el->setAttribute('type','hidden');
  $el->setAttribute('id','jsinputchannels');
  $el->setAttribute('name','jsinputchannels');
  $el->setAttribute('value',json_encode($devprop['inputchannels']));
  // pass hw channel list to javascript:
  $el = $form->appendChild($doc->createElement('input'));
  $el->setAttribute('type','hidden');
  $el->setAttribute('id','hwinputchannels');
  $el->setAttribute('value',json_encode($devprop['hwinputchannels']));
  $div = $form->appendChild($doc->createElement('div'));
  $div->setAttribute('id','jsinputchannelsdiv');
  // load java scripts:
  $script = $form->appendChild($doc->createElement('script'));
  $script->appendChild($doc->createTextNode(file_get_contents( '../js/inputchannels.js' )));
}

function get_dev_stage_settings( $device )
{
  $d = array();
  $dprop = get_properties($device,'device');
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
  return $d;
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
  $rendersettings['renderreverb'] = boolval($devprop['reverb']);
  $rendersettings['renderism'] = boolval($devprop['renderism']);
  $rendersettings['mastergain'] = pow(10.0,0.05*$devprop['mastergain']);
  $rendersettings['rectype'] = $devprop['rectype'];
  $rendersettings['secrec'] = floatval($devprop['secrec']);
  $rendersettings['rawmode'] = boolval($devprop['rawmode']);
  $rendersettings['xrecport'] = $devprop['xrecport'];
  $rendersettings['headtracking'] = boolval($devprop['headtracking']);
  $rendersettings['headtrackingrot'] = boolval($devprop['headtrackingrot']);
  $rendersettings['headtrackingrotsrc'] = boolval($devprop['headtrackingrotsrc']);
  $rendersettings['headtrackingport'] = intval($devprop['headtrackingport']);
  $rendersettings['extracfg'] = array('headtrack'=>array('load'=>boolval($devprop['headtracking']),
                                                         'applyrec'=>boolval($devprop['headtrackingrot']),
                                                         'applysrc'=>boolval($devprop['headtrackingrotsrc']),
                                                         'port'=>intval($devprop['headtrackingport']),
                                                         'tauref'=>floatval($devprop['headtrackingtauref'])),
                                      'monitor'=>array('delay'=>floatval($devprop['selfmonitordelay'])),
                                      'metronome'=>array('active'=>boolval($devprop['metroactive']),
                                                         'bpm'=>floatval($devprop['metrobpm']),
                                                         'bpb'=>intval($devprop['metrobpb']),
                                                         'level'=>floatval($devprop['metrolevel']),
                                                         'delay'=>floatval($devprop['metrodelay'])),
                                      'proxy'=>array('isproxy'=>boolval($devprop['isproxy']),
                                                     'useproxy'=>boolval($devprop['useproxy'])));
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
  $audio['restart'] = boolval($devprop['jackrestart']);
  if( $devprop['jackrestart'] )
    modify_device_prop( $device, 'jackrestart', false );
  $room = array('host'=>'','port'=>0,'pin'=>0);
  $roomdev = array();
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
                             'gain'=>pow(10.0,0.05*($roomprop['rvbgain']+$devprop['rvbgain'])));
      // get all devices in room:
      $rdevs = get_devices_in_room( $devprop['room'] );
      foreach( $rdevs as $rdev ){
        $roomdev[] = get_dev_stage_settings( $rdev );
      }
    }
  }
  $stagecfg = array('rendersettings'=>$rendersettings,
                    'audiocfg'=>$audio,
                    'room'=>$room,
                    'roomdev'=>$roomdev,
                    'firmwareupdate'=>$devprop['firmwareupdate'],
                    'usedevversion'=>$devprop['usedevversion']);
  if( $devprop['firmwareupdate'] )
    modify_device_prop( $device, 'firmwareupdate', false );
  if( $devprop['usedevversion'] )
    modify_device_prop( $device, 'usedevversion', false );
  if( !empty($devprop['frontendconfig']) ){
    $stagecfg['frontendconfig'] = $devprop['frontendconfig'];
    modify_device_prop( $device, 'frontendconfig', array() );
  }
  $stagecfg = json_encode( $stagecfg );
  $hash = hash('md5',$stagecfg);
  if( $hash != $devhash ){
    echo $hash . "\n";
    echo $stagecfg;
  }
}

function checkPassword($pwd, &$errors) {
  $errors_init = $errors;
  if (strlen($pwd) < 6) {
    $errors[] = "Password too short!";
  }
  if (!preg_match("#[0-9]+#", $pwd)) {
    $errors[] = "Password must include at least one number!";
  }
  if (!preg_match("#[a-zA-Z]+#", $pwd)) {
    $errors[] = "Password must include at least one letter!";
  }     
  return ($errors == $errors_init);
}

function request_passwd_reset( $username )
{
  cleanup_registrations();
  $users = list_users();
  if( !in_array( $username, $users ) )
    return;
  $uprop = get_properties($username,'user');
  if( empty($uprop['mail']))
    return;
  $mail = $uprop['mail'];
  $reghash = md5(random_bytes(5) . $mail );
  set_properties( $reghash, 'pwreset', array('m'=>$mail,'u'=>$username) );
  $actual_link = (isset($_SERVER['HTTPS']) && $_SERVER['HTTPS'] === 'on' ? "https" : "http") . "://$_SERVER[HTTP_HOST]$_SERVER[REQUEST_URI]";
  $mailmsg = "Please follow this link to reset the password of your ovbox acount ".$username.":\n\n" . $actual_link . "&pwreset=".$reghash .
    "\n\nThis link is valid for one hour.\n\nIf you did not request a password reset, please ignore this mail.\n";
  file_put_contents( '../lastmailsg',$mailmsg);
  mail( $mail, 'ovbox password reset', $mailmsg, array('From'=>'noreply@orlandoviols.com') );
}

function register_new_user( $mail, $username, $password, $group, &$msg )
{
  if( empty($mail) ){
    $msg = 'No mail address was provided.';
    return false;
  }
  if( empty($username) ){
    $msg = 'No user name was provided.';
    return false;
  }
  if( empty($password) ){
    $msg = 'No password was provided.';
    return false;
  }
  $errs = array();
  if( !checkPassword($password,$errs) ){
    foreach($errs as $err)
      $msg = $msg . '<br/>'.$err;
    return false;
  }
  if( !filter_var( $mail, FILTER_VALIDATE_EMAIL ) ){
    $msg = '"'.htmlentities($mail).'" is not a valid email address.';
    return false;
  }
  cleanup_registrations();
  $registr = list_db('registration');
  $regcnt = 0;
  foreach( $registr as $rh=>$regdb )
    if( $regdb['m'] == $mail )
      $regcnt = $regcnt + 1;
  if( $regcnt > 2 ){
    $msg = 'Too many registrations for this email address. Please try again later.';
    return false;
  }
  $users = list_users();
  if( in_array( $username, $users ) ){
    $msg = 'The user "'.$username.'" already exists. Please chose another user name.';
    return false;
  }
  $reghash = md5(random_bytes(5) . $mail );
  set_properties( $reghash, 'registration', array('m'=>$mail,'u'=>$username,'g'=>$group,'p'=>password_hash( $password, PASSWORD_DEFAULT )) );
  $actual_link = (isset($_SERVER['HTTPS']) && $_SERVER['HTTPS'] === 'on' ? "https" : "http") . "://$_SERVER[HTTP_HOST]$_SERVER[REQUEST_URI]";
  $mailmsg = "Please follow this link to activate your ovbox acount ".$username.":\n\n" . $actual_link . "&activate=".$reghash .
    "\n\nThis link is valid for one hour.\n\nIf you did not request an activation, please ignore this mail.\n";
  $site = get_properties('site','config');
  if( $site['moderation'] )
    $mailmsg = $mailmsg."\n"."from ".$mail."\n";
  file_put_contents( '../lastmailsg',$mailmsg);
  if( $site['moderation'] )
    mail( $site['mail'], 'ovbox account activation request', $mailmsg, array('From'=>'noreply@orlandoviols.com') );
  else
    mail( $mail, 'ovbox account activation', $mailmsg, array('From'=>'noreply@orlandoviols.com') );
  return true;
}

function update_pw( $password, $username, &$msg )
{
  if( empty($username) ){
    $msg = 'No user name was provided.';
    return false;
  }
  if( empty($password) ){
    $msg = 'No password was provided.';
    return false;
  }
  $errs = array();
  if( !checkPassword($password,$errs) ){
    foreach($errs as $err)
      $msg = $msg . '<br/>'.$err;
    return false;
  }
  $users = list_users();
  if( !in_array( $username, $users ) ){
    $msg = 'The user "'.$username.'" does not exist.';
    return false;
  }
  $pwhash = password_hash( $password, PASSWORD_DEFAULT );
  $fname = '../db/ovpasswd';
  $pws = array();
  if( file_exists( $fname ) )
    $pws = json_decode( file_get_contents( $fname ), true );
  $pws[$username] = $pwhash;
  file_put_contents( $fname, json_encode( $pws ));
  modify_user_prop( $username, 'validpw', true);
  return true;
}

function validate_pwreset( $acthash, &$pwr )
{
  $fname = '../db/'.$acthash.'.pwreset';
  if( !file_exists( $fname ) )
    return false;
  if( time()-filemtime($fname) > 3600 ){
    unlink($fname);
    return false;
  }
  $reg = get_properties($acthash,'pwreset');
  unlink($fname);
  modify_user_prop( $reg['u'], 'validpw', false);
  $pwr = $reg['u'];
  cleanup_registrations();
  return true;
}

function activate_new_user( $acthash )
{
  $fname = '../db/'.$acthash.'.registration';
  if( !file_exists( $fname ) )
    return false;
  if( time()-filemtime($fname) > 3600 ){
    unlink($fname);
    return false;
  }
  $reg = get_properties($acthash,'registration');
  unlink($fname);
  $users = list_users();
  if( in_array( $reg['u'], $users ) )
    return false;
  $fname = '../db/ovpasswd';
  $pws = array();
  if( file_exists( $fname ) )
    $pws = json_decode( file_get_contents( $fname ), true );
  $pws[$reg['u']] = $reg['p'];
  file_put_contents( $fname, json_encode( $pws ));
  if( in_array($reg['g'],list_groups()) ){
    modify_user_prop( $reg['u'], 'maingroup', $reg['g']);
    add_user_to_group($reg['u'],$reg['g']);
  }
  modify_user_prop( $reg['u'], 'mail', $reg['m']);
  modify_user_prop( $reg['u'], 'agreedterms', true);
  modify_user_prop( $reg['u'], 'agreedprivacy', true);
  modify_user_prop( $reg['u'], 'access', time() );
  cleanup_registrations();
  return true;
}

function cleanup_registrations()
{
  foreach( list_db('registration') as $r=>$dpro ){
    $fname = '../db/'.$r.'.registration';
    if( time()-filemtime($fname) > 3600 )
      unlink($fname);
  }
  foreach( list_db('pwreset') as $r=>$dpro ){
    $fname = '../db/'.$r.'.pwreset';
    if( time()-filemtime($fname) > 3600 )
      unlink($fname);
  }
}

function terminate_account( $user )
{
  $fname = '../db/ovpasswd';
  $pws = array();
  if( file_exists( $fname ) )
    $pws = json_decode( file_get_contents( $fname ), true );
  if( !array_key_exists($user,$pws) ){
    $sitecfg = get_properties('site','config');
    mail( $sitecfg['mail'], 'manual ovbox account activation request', 'remove user '.$user, array('From'=>'noreply@orlandoviols.com') );
  }
  unset($pws[$user]);
  file_put_contents( $fname, json_encode( $pws ));
  foreach( list_devices() as $dev=>$dprop){
    if( $dprop['owner'] == $user )
      rm_device( $dev );
  }
  foreach( list_db('room') as $room=>$dprop){
    if( $dprop['owner'] == $user )
      modify_room_prop( $room, 'owner', '' );
  }
  $fname = '../db/' . $user . '.user';
  if( file_exists( $fname ) )
    unlink($fname);
  cleanup_groups();
}

// APR1-MD5 encryption method (windows compatible)
function crypt_apr1_md5($plainpasswd, $salt)
{
    $tmp = "";
    $len = strlen($plainpasswd);
    $text = $plainpasswd.'$apr1$'.$salt;
    $bin = pack("H32", md5($plainpasswd.$salt.$plainpasswd));
    for($i = $len; $i > 0; $i -= 16) { $text .= substr($bin, 0, min(16, $i)); }
    for($i = $len; $i > 0; $i >>= 1) { $text .= ($i & 1) ? chr(0) : $plainpasswd{0}; }
    $bin = pack("H32", md5($text));
    for($i = 0; $i < 1000; $i++)
    {
        $new = ($i & 1) ? $plainpasswd : $bin;
        if ($i % 3) $new .= $salt;
        if ($i % 7) $new .= $plainpasswd;
        $new .= ($i & 1) ? $bin : $plainpasswd;
        $bin = pack("H32", md5($new));
    }
    for ($i = 0; $i < 5; $i++)
    {
        $k = $i + 6;
        $j = $i + 12;
        if ($j == 16) $j = 5;
        $tmp = $bin[$i].$bin[$k].$bin[$j].$tmp;
    }
    $tmp = chr(0).chr(0).$bin[11].$tmp;
    $tmp = strtr(strrev(substr(base64_encode($tmp), 2)),
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/",
    "./0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz");
 
    return "$"."apr1"."$".$salt."$".$tmp;
}

function get_htpasswd( $passwdFile, $username )
{
    $lines = file($passwdFile);
    foreach ($lines as $line)
    {
        $arr = explode(":", $line);
        $fileUsername = $arr[0];
        if ($fileUsername == $username)
        {
            $filePasswd = trim($arr[1]);
            return $filePasswd;
        }
    }
    return false;
}

function htpw_matches($password, $filePasswd)
{
    if (strpos($filePasswd, '$apr1') === 0)
    {
        // MD5
        $passParts = explode('$', $filePasswd);
        $salt = $passParts[2];
        $hashed = crypt_apr1_md5($password, $salt);
        return $hashed == $filePasswd;
    }
    elseif (strpos($filePasswd, '{SHA}') === 0)
    {
        // SHA1
        $hashed = "{SHA}" . base64_encode(sha1($password, TRUE));
        return $hashed == $filePasswd;
    }
    elseif (strpos($filePasswd, '$2y$') === 0)
    {
       // Bcrypt
       return password_verify ($password, $filePasswd);
    }
    else
    {
        // Crypt
        $salt = substr($filePasswd, 0, 2);
        $hashed = crypt($password, $salt);
        return $hashed == $filePasswd;
    }
    return false;
}

function submit_contact( $user, $contact, $msg )
{
  $mailmsg = 'From: '.$user.' ('.$contact.")\n\n".$msg."\n";
  file_put_contents( '../lastmailsg',$mailmsg);
  $sitecfg = get_properties('site','config');
  mail( $sitecfg['mail'], 'ovbox contact from '.$user, $mailmsg, array('From'=>'noreply@orlandoviols.com') );
}

function cleanup_rooms()
{
  $rooms = list_db('room');
  foreach( list_db('room') as $room=>$rprop ){
    // only operate on unmanaged rooms which were not entered within the last hour:
    if( empty($rprop['owner']) && ($rprop['enterage']>3600)){
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

function claim_device_id( $user, $dev,  &$msg )
{
  $device = str_replace( ":", "", strtolower( $dev ) );
  $fname = '../db/'.$device.'.device';
  if( file_exists( $fname ) ){
    // the user entered a listed device ID:
    $dprop = get_properties( $device, 'device' );
    if( $dprop['age'] > 20 ){
      $msg = 'The device '.$device.' is currently not active. Please power on your device to claim it. It may take up to one minute until the device is started properly.';
      return false;
    }
    if( $dprop['onlineage'] > 300 ){
      $msg = 'The device '.$device.' is online for more than five minutes. Please claim your device within the first five minutes after powering on.';
      return false;
    }
    unlink($fname);
    modify_device_prop( $device, 'version', $dprop['version']);
    modify_device_prop( $device, 'owner', $user);
    modify_device_prop( $device, 'access', $dprop['access']);
    modify_device_prop( $device, 'onlineaccess', $dprop['onlineaccess']);
    return true;
  }else{
    $msg = 'No device with MAC address or device ID '.$dev.' found in the data base. You may use the contact form to send date and time of installation, to ask for a manual identification.';
    return false;
  }
}

/*
 * Local Variables:
 * c-basic-offset: 2
 * mode: php
 * End:
 */

?>
