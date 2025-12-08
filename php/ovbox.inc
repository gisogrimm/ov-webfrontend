<?php

// internationalization:
$langs = get_languages();
$GLOBALS['browserlang'] = 'US';
if( array_key_exists( 'HTTP_ACCEPT_LANGUAGE', $_SERVER ) ){
foreach (explode(',', $_SERVER['HTTP_ACCEPT_LANGUAGE']) as $lang) {
  $pattern = '/^(?P<primarytag>[a-zA-Z]{2,8})'.
           '(?:-(?P<subtag>[a-zA-Z]{2,8}))?(?:(?:;q=)'.
           '(?P<quantifier>\d\.\d))?$/';
    
  $splits = array();
  if (preg_match($pattern, $lang, $splits)) {
    if( array_key_exists('subtag',$splits) ){
      if( in_array( strtoupper($splits['subtag']), $langs ) ){
        $GLOBALS['browserlang'] = strtoupper($splits['subtag']);
        break;
      }
    }else{
      if( array_key_exists('primarytag',$splits) ){
        if( in_array( strtoupper($splits['primarytag']), $langs ) ){
          $GLOBALS['browserlang'] = strtoupper($splits['primarytag']);
          break;
        }
      }
    }
  }
}
}
if( !array_key_exists( 'langdb', $GLOBALS) ){
  // user browser settings only if no session
  $GLOBALS['userlang'] = $GLOBALS['browserlang'];
  $GLOBALS['langdb'] = array();
  $fname = '../db/'.$GLOBALS['userlang'].'.lang';
  if( file_exists( $fname ) ){
    $filec = json_decode( file_get_contents( $fname ), true );
    if( is_array($filec) ){
      $GLOBALS['langdb'] = array_merge($GLOBALS['langdb'], $filec);
    }
  }
}
// end internationalization.

$fp_dev = fopen("../lock_dev.txt", "a+");
$fp_user = fopen("../lock_user.txt", "a+");
$fp_register = fopen("../lock_reg.txt", "a+");
$fp_authfail = fopen("../lock_auth.txt", "a+");

function defaults( $type )
{
  switch( $type ){
  case 'sortkey':
    return array(
      'user'=>'id',
      'room'=>'id',
      'device'=>'id',
    );
    break;
  case 'config':
    return array(
      'mail'=>'',
      'admin'=>array(),
      'subscriptionadmin'=>array(),
      'pagetitle'=>'House of Consort',
      'basecss'=>'ovbox.css',
      'credits'=>'Background image by Johannes Gontarski.',
      'moderation'=>false,
      'forcehttps'=>true,
      'subscriptionrate'=>3.0,
    );
    break;
  case 'style':
    return array(
      'credits'=>'',
      'css'=>'ovbox.css',
      'pagetitle'=>'House of Consort',
      'showdonate'=>true,
    );
    break;
  case 'device':
    return array(
      'inittime'=>'',
      'access'=>0,
      'logaccess'=>0,
      'onlineaccess'=>0,
      'chair'=>-1,
      'label'=>'',
      'owner'=>'',
      'version'=>'',
      'selfmonitor'=>true,
      'egogain'=>-6,
      'selfmonitordelay'=>0,
      'selfmonitoronlyreverb'=>false,
      'host'=>'',
      'localip'=>'',
      'externalip'=>'',
      'inputchannels'=>array(array(
        'sourceport'=>'system:capture_1',
        'gain'=>1,
        'position'=>array('x'=>0,'y'=>0,'z'=>0),
        'directivity'=>'omni',
        'plugins'=>array(),
        'name'=>'',
      )),
      'jitterreceive'=>5,
      'jittersend'=>5,
      'expeditedforwarding'=>false,
      'outputport1'=>'system:playback_1',
      'outputport2'=>'system:playback_2',
      'xport'=>'{}',
      'nozita'=>false,
      'peer2peer'=>true,
      'usetcptunnel'=>false,
      'encryption'=>true,
      'secrec'=>0,
      'xrecport'=>array(),
      'sessionportoffset'=>0,
      'position'=>array('x'=>0,'y'=>0,'z'=>0),
      'orientation'=>array('z'=>0,'y'=>0,'x'=>0),
      'srcshiftxyz'=>'0 0 0',
      'virtualacoustics'=>true,
      'receive'=>true,
      'reverb'=>true,
      'renderism'=>false,
      'rendersoundscape'=>true,
      'distancelaw'=>false,
      'delaycomp'=>2.4,
      'rvbgain'=>0,
      'mastergain'=>0,
      'playbackgain'=>0,
      'room'=>'',
      'roomenter'=>0,
      'rectype'=>'hrtf',
      'decorr'=>0,
      'fdnforwardstages'=>0,
      'fdnorder'=>5,
      'jackdevice'=>'highest',
      'jackplugdev'=>false,
      'jackrate'=>48000,
      'jackperiod'=>96,
      'jackbuffers'=>2,
      'jackpriority'=>40,
      'jackrestart'=>false,
      'alsadevs'=>array(),
      'bandwidth'=>array('tx'=>0,'rx'=>0),
      'cpuload'=>0,
      'thermal'=>array(),
      'lmetertc'=>0.5,
      'lmeterfw'=>'Z',
      'headtracking'=>false,
      'headtrackingserial'=>false,
      'headtrackingrot'=>true,
      'headtrackingrotsrc'=>true,
      'headtrackingport'=>0,
      'headtrackingtauref'=>33.315,
      'headtrackingtilturl'=>'',
      'headtrackingtiltpath'=>'/tilt',
      'headtrackingtiltmap'=>'0 0 180 180',
      'headtrackingeogpath'=>'',
      'headtrackingautorefzonly'=>false,
      'sendlocal'=>true,
      'hwinputchannels'=>array('system:capture_1','system:capture_2'),
      'firmwareupdate'=>false,
      'firmwareupdategit'=>false,
      'installopenmha'=>false,
      'isupdating'=>false,
      'usedevversion'=>false,
      'isovbox'=>true,
      'lastfrontendconfig', array(),
      'frontendconfig', array(),
      'nochair'=>false,
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
      'wifi'=>false,
      'wifissid'=>'',
      'wifipasswd'=>'',
      'wifiupdate'=>false,
      'usehifiberry'=>'',
      'receivedownmix'=>false,
      'senddownmix'=>false,
      'start_webmixer'=>true,
      'tscinclude'=>'',
      'mhaconfig'=>'',
      'jackrecsampleformat'=>'PCM_16',
      'jackrecfileformat'=>'WAV',
      'showmczita'=>false,
      'uselocmcrec'=>false,
      'uselocmcsend'=>false,
      'locmcrecaddr'=>'239.255.0.10',
      'locmcrecport'=>10007,
      'locmcrecdevice'=>'',
      'locmcrecbuffer'=>10,
      'locmcrecchannels'=>[1,2],
      'locmcrecautoconnect'=>false,
      'locmcsendchannels'=>2,
      'emptysessionismonitor'=>false,
      'usebcf2000'=>false,
      'snmon_rvb_sx'=>25,
      'snmon_rvb_sy'=>13,
      'snmon_rvb_sz'=>7.5,
      'snmon_rvb_abs'=>0.6,
      'snmon_rvb_damp'=>0.7,
      'tuner_f0'=>440.0,
      'tuner_tuning'=>'equal',
      'tuner_active'=>false,
      'networkdevices'=>array(),
      'zitasampleformat'=>'16bit',
      'useloudspeaker'=>false,
      'echoc_nrep'=>64,
      'echoc_level'=>60.0,
      'echoc_maxdist'=>4.0,
      'echoc_filterlen'=>65,
      'hiresping'=>false,
      'uname_sysname'=>'',
      'uname_release'=>'',
      'uname_machine'=>'',
      'encryptstate'=>0,
      'showdevlog'=>false,
      'standalonemixer'=>false,
    );
    break;
  case 'room':
    return array(
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
      'premium'=>false,
      'clearowned'=>false,
      'version'=>'',
    );
    break;
  case 'user':
    return array(
      'group'=>'',
      'seesall'=>true,
      'maingroup'=>'',
      'access'=>0,
      'agreedterms'=>false,
      'agreedprivacy'=>false,
      'validpw'=>true,
      'mail'=>'',
      'devverwarn'=>0,
      'subscription'=>false,
      'subscriptionend'=>0,
      'allowninja'=>false,
      'institution'=>false,
      'lang'=>$GLOBALS['browserlang'],
    );
    break;
  case 'group':
    return array(
      'style'=>'default',
      'preamble'=>'<div>The <em>ovbox</em> is a remote music collaboration system developed by
the ORLANDOviols ensemble during the Covid19 pandemic.</div>'."\n"
    );
    break;
  }
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
  if( array_key_exists('logaccess',$prop) )
    $prop['logage'] = time()-$prop['logaccess'];
  if( array_key_exists('onlineaccess',$prop) )
    $prop['onlineage'] = time()-$prop['onlineaccess'];
  if( $type == 'device' ){
    $prop['rawmode'] = !boolval($prop['virtualacoustics']);

    $prop['egogain'] = floatval($prop['egogain']);
    foreach(array_keys($prop) as $key){
      if( is_numeric($key) )
        unset($prop[$key]);
    }
    if( empty($prop['label']) && (!empty($prop['owner'])))
      $prop['label'] = $prop['owner'];
    $prop['label'] = preg_replace('/[^a-zA-Z0-9\-_]*/','',$prop['label']);
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
  if( $type == 'user' ){
    $prop['validsubscription'] = validsubscription( $prop );
  }
}

function get_properties( $id, $type )
{
  $fname = '../db/'.$id.'.'.$type;
  $prop = defaults($type);
  if( !empty($id) && file_exists( $fname ) ){
    $filec = json_decode( file_get_contents( $fname ), true );
    if( is_array($filec) )
      $prop = array_merge($prop, $filec);
  }
  postproc_prop( $prop, $type );
  return $prop;
}

function set_properties( $id, $type, $dprop )
{
  $fname = '../db/'.$id.'.'.$type;
  if( !empty($id) )
    if( is_object( $dprop )||is_array($dprop) )
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
    if( ($devprop['age'] < 20) && empty($devprop['owner']) && ($devprop['onlineage'] < 600) && ($dev != '000000000000') ){
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
    if( $devprop['owner'] == $user ){
      $devprop['id'] = $dev;
      $devs[$dev] = $devprop;
    }
  }
  uasort( $devs, function ( $a, $b )
  {
    // sort inactive devices by age, and active devices by label:
    $age_a = max($a['age'],30);
    $age_b = max($b['age'],30);
    if( $age_a > $age_b )
      return 1;
    if( $age_a < $age_b )
      return -1;
    // max(age,30) is the same, i.e., device is active or the age is
    // the same, thus sort by label:
    if( $a['label'] > $b['label'] )
      return 1;
    if( $a['label'] < $b['label'] )
      return -1;
    // the labels are identical, therefore sort by device ID:
    if( $a['id'] > $b['id'] )
      return 1;
    if( $a['id'] < $b['id'] )
      return -1;
    return 0;
  } );
  return $devs;
}

function select_first_active_dev( $user )
{
  if( !isset($alldev) )
    $alldev = list_devices();
  foreach( $alldev as $dev=>$devprop ){
    if( ($devprop['owner'] == $user) && ($devprop['age']<20) ){
      select_userdev( $user, $dev );
      return;
    }
  }
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

function validsubscription( $userprop )
{
  if( $userprop['subscription'] )
    return true;
  return $userprop['subscriptionend'] > $userprop['now'];
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
                      'position'=>$prop['position'],
                      'orientation'=>$prop['orientation'],
                      'issender'=>issender($prop),
                      'jittersend'=>floatval($prop['jittersend']),
                      'jitterreceive'=>floatval($prop['jitterreceive']),
                      'jackrate'=>floatval($prop['jackrate']),
                      'jackperiod'=>floatval($prop['jackperiod']),
                      'version'=>$prop['version'],
                      'numchannels'=>count($prop['inputchannels']),
                      'inputchannels'=>$prop['inputchannels'],
                      'senddownmix'=>boolval($prop['senddownmix']),
                      'receivedownmix'=>boolval($prop['receivedownmix']),
                      'isproxy'=>boolval($prop['isproxy']),
                      'useproxy'=>boolval($prop['useproxy']),
                      'peer2peer'=>boolval($prop['peer2peer']),
                      'encryptstate'=>intval($prop['encryptstate']));
    if( ($room == $prop['room']) && ((!$senderonly) || (issender($prop) && (!$prop['senddownmix']))) ){
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
    if( is_object( $groups )||is_array($groups) )
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
          if( is_object( $groups )||is_array($groups) )
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
          if( is_object( $groups )||is_array($groups) )
            file_put_contents( $fname, json_encode( $groups ));
        }
      }
    }
  }
}


// internationalization:

function get_languages()
{
  $languages = array();
  foreach( glob( '../db/*.lang' ) as $language_file){
    $language = basename( $language_file, '.lang' );
    $languages[] = $language;
  }
  ksort( $languages );
  return $languages;
}

function translate( $from )
{
  if( array_key_exists( 'langdb', $GLOBALS) ){
    if( array_key_exists( $from, $GLOBALS['langdb'] ) )
      return $GLOBALS['langdb'][$from];
    if( $GLOBALS['userlang'] != 'US' )
      error_log('missing translation for "'.$from.'" for language "'.$GLOBALS['userlang'].'"');
  }
  return $from;
}

function checkUsername($usrn, &$errors) {
  $errors_init = $errors;
  if (strlen($usrn) < 3) {
    $errors[] = "User name too short!";
  }
  if (preg_match("#[[:blank:]]+#", $usrn)) {
    $errors[] = "User name can not contain spaces!";
  }
  return ($errors == $errors_init);
}

function request_passwd_reset( $username )
{
  cleanup_registrations();
  $users = list_users();
  if( !in_array( $username, $users ) )
    return "";
  $uprop = get_properties($username,'user');
  //if( empty($uprop['mail']))
  //  return;
  //$mail = $uprop['mail'];
  $reghash = md5(random_bytes(5) . $username );
  set_properties( $reghash, 'pwreset', array('u'=>$username) );
  //$actual_link = (isset($_SERVER['HTTPS']) && $_SERVER['HTTPS'] === 'on' ? "https" : "http") . "://$_SERVER[HTTP_HOST]$_SERVER[REQUEST_URI]";
  $actual_link = (isset($_SERVER['HTTPS']) && $_SERVER['HTTPS'] === 'on' ? "https" : "http") . "://$_SERVER[HTTP_HOST]";
  //$mailmsg = "Please follow this link to reset the password of your ovbox acount ".$username.":\n\n" . $actual_link . "&pwreset=".$reghash .
  //         "\n\nThis link is valid for one hour.\n\nIf you did not request a password reset, please ignore this mail.\n";
  //file_put_contents( '../lastmailsg',$mailmsg);
  //mail( $mail, 'ovbox password reset', $mailmsg, array('From'=>'info@ovbox.de') );
  return $actual_link . "/login.php?pwreset=" . $reghash;//file_put_contents( '../lastmsg',$msg);
}

function cleanup_registrations()
{
  foreach( list_db('registration') as $r=>$dpro ){
    $fname = '../db/'.$r.'.registration';
    if( time()-filemtime($fname) > 3600*24*3 )
      unlink($fname);
  }
  foreach( list_db('pwreset') as $r=>$dpro ){
    $fname = '../db/'.$r.'.pwreset';
    if( time()-filemtime($fname) > 3600*24*3 )
      unlink($fname);
  }
}

/*
 * Local Variables:
 * c-basic-offset: 2
 * mode: php
 * End:
 */

?>
