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
    'externalip'=>'',
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
    'proxyip'=>'',
    'showexpertsettings'=>false,
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

function get_properties( $id, $type )
{
  $fname = '../db/'.$id.'.'.$type;
  $prop = defaults($type);
  if( !empty($id) && file_exists( $fname ) )
    $prop = array_merge($prop, json_decode( file_get_contents( $fname ), true ));
  postproc_prop( $prop, $type );
  return $prop;
}

function set_properties( $id, $type, $dprop )
{
  $fname = '../db/'.$id.'.'.$type;
  if( !empty($id) )
    file_put_contents( $fname, json_encode( $dprop ));
}

function rm_device( $dev )
{
  $fname = '../db/' . $dev . '.device';
  unlink($fname);
}

function modify_device_prop( $device, $key, $value )
{
  $dprop = get_properties( $device, 'device' );
  $dprop[$key] = $value;
  set_properties( $device, 'device', $dprop );
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

function select_userdev( $user, $dev )
{
  $fname = '../db/' . $user . '.userdevice';
  if( empty($dev) ){
    unlink($fname);
  }else{
    file_put_contents( $fname, $dev );
  }
}


/*
 * Local Variables:
 * c-basic-offset: 2
 * mode: php
 * End:
 */

?>
