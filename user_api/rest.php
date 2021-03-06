<?php

if( !(isset($_SERVER['HTTPS']) && $_SERVER['HTTPS'] === 'on') ){
    if( substr_compare( $_SERVER['HTTP_HOST'], 'localhost', 0, 9 )!= 0){
        $actual_link = "https://$_SERVER[HTTP_HOST]$_SERVER[REQUEST_URI]";
        header( "Location: ".$actual_link );
        die();
    }
}

include '../php/ovbox.inc';
include '../php/rest.inc';

session_start();
if( !isset($_SESSION['user']) )
    die();
$user = $_SESSION['user'];
flock($fp_user, LOCK_EX );
if( !in_array($user,list_users()) ){
    flock($fp_user, LOCK_UN );
    die();
}
$uprop = get_properties( $user, 'user' );
if( isset($_GET['getuser']) ){
    echo(json_encode($uprop));
    flock($fp_user, LOCK_UN );
    die();
}
flock($fp_dev, LOCK_EX );
$device = get_device( $user );
flock($fp_user, LOCK_UN );
$dprop = get_properties( $device, 'device' );
if( $dprop['owner'] != $user ){
    $device = '';
    $dprop = get_properties( $device, 'device' );
}
if( empty($dprop['owner']) )
    $dprop['owner'] = $user;
if( isset($_GET['getdev']) ){
    unset($dprop['outputport1']);
    unset($dprop['outputport2']);
    unset($dprop['firmwareupdate']);
    echo(json_encode($dprop));
    flock($fp_dev, LOCK_UN );
    die();
}
if( isset($_GET['metroactive']) ){
    modify_device_prop($device,'metroactive',$_GET['metroactive']=='true');
}
if( isset($_GET['metrobpm']) ){
    modify_device_prop($device,'metrobpm',floatval($_GET['metrobpm']));
}
if( isset($_GET['metrobpb']) ){
    modify_device_prop($device,'metrobpb',intval($_GET['metrobpb']));
}
if( isset($_GET['metrodelay']) ){
    modify_device_prop($device,'metrodelay',floatval($_GET['metrodelay']));
}
if( isset($_GET['metrolevel']) ){
    modify_device_prop($device,'metrolevel',floatval($_GET['metrolevel']));
}
if( isset($_GET['useproxy']) ){
    modify_device_prop($device,'useproxy',$_GET['useproxy']=='true');
}
if( isset($_GET['isproxy']) ){
    modify_device_prop($device,'isproxy',$_GET['isproxy']=='true');
}
flock($fp_dev, LOCK_UN );
if( isset($_GET['getrooms']) ){
    $usergroups = list_groups($user);
    $dprop['id'] = $device;
    $dprop['issender'] = issender($dprop);
    $dprop['usergroups'] = $usergroups;
    $jsrooms = array('user'=>$user,
                     'device'=>$dprop,
                     'owned_devices'=>owned_devices($user),
                     'rooms'=>get_rooms_user( $user, $uprop, $usergroups, $dprop['room'] ),
                     'unclaimed_devices'=>list_unclaimed_devices());
    echo(json_encode($jsrooms));
}
$presetkeys = ['label',
               'selfmonitor',
               'egogain',
               'inputchannels',
               'jitterreceive',
               'jittersend',
               'outputport1',
               'outputport2',
               'xport',
               'peer2peer',
               'secrec',
               'xrecport',
               'rawmode',
               'reverb',
               'renderism',
               'rvbgain',
               'mastergain',
               'playbackgain',
               'rectype',
               'isproxy',
               'useproxy',
               'jackdevice',
               'jackplugdev',
               'jackrate',
               'jackperiod',
               'jackbuffers',
               'headtracking',
               'headtrackingrot',
               'headtrackingrotsrc',
               'headtrackingport',
               'headtrackingtauref',
               'sendlocal',
               'isproxy',
               'useproxy',
               'showexpertsettings'];
if( isset($_GET['devpresetsave']) ){
    if( !empty($_GET['devpresetsave']) ){
        $presets = get_properties( $device, 'devpresets' );
        $preset = array();
        foreach( $presetkeys as $key )
            $preset[$key] = $dprop[$key];
        $presets[$_GET['devpresetsave']] = $preset;
        set_properties( $device, 'devpresets', $presets );
        modify_device_prop($device,'preset',$_GET['devpresetsave']);
    }
}
if( isset($_GET['devpresetload']) ){
    $presets = get_properties( $device, 'devpresets' );
    if( array_key_exists( $_GET['devpresetload'], $presets ) ){
        $preset = $presets[$_GET['devpresetload']];
        foreach( $preset as $key=>$value )
            $dprop[$key]=$value;
        set_properties($device,'device',$dprop);
        modify_device_prop($device,'preset',$_GET['devpresetload']);
    }
}
if( isset($_GET['devpresetrm']) ){
    $presets = get_properties( $device, 'devpresets' );
    if( array_key_exists( $_GET['devpresetrm'], $presets ) ){
        unset( $presets[$_GET['devpresetrm']]);
        set_properties( $device, 'devpresets', $presets );
        if( $dprop['preset'] == $_GET['devpresetrm'] )
            modify_device_prop($device,'preset','');
    }
}
if( isset($_POST['jackaudio']) ){
    $dprop['jackplugdev'] = isset($_POST['jackplugdev']) && ($_POST['jackplugdev']=='true');
    set_getprop_post($dprop,'jackdevice');
    set_getprop_post_float($dprop,'jackrate');
    set_getprop_post_float($dprop,'jackperiod');
    set_getprop_post_float($dprop,'jackbuffers');
    $dprop['preset'] = '';
    set_properties( $device, 'device', $dprop );
}
if( isset($_GET['getrawjson']) ){
    header('Content-Type: application/json');
    echo(json_encode($dprop,JSON_PRETTY_PRINT));
}
if( isset($_GET['devselect']) ){
    select_userdev( $user, $_GET['devselect'] );
}
if( isset($_GET['primarygroup']) ){
    if( empty($_GET['primarygroup']) || in_array($_GET['primarygroup'],list_groups($user)) ){
        modify_user_prop( $user, 'maingroup', $_GET['primarygroup'] );
    }
}
if( isset($_GET['usermail']) )
    modify_user_prop( $user, 'mail', $_GET['usermail'] );
if( isset($_POST['usermail']) )
    modify_user_prop( $user, 'mail', $_POST['usermail'] );
if( isset($_POST['agreepriv']) )
    modify_user_prop( $user, 'agreedprivacy', true );
if( isset($_POST['agreeterms']) )
    modify_user_prop( $user, 'agreedterms', true );
if( isset($_POST['updatepassword']) ){
    $msg = '';
    update_pw( $_POST['updatepassword'], $user, $msg );
    if( !empty($msg) )
        $_SESSION['usermsg'] = 'Invalid password:'.$msg;
}
if( isset($_POST['mypwreset']) ){
    modify_user_prop( $user, 'validpw', false);
}
if( isset($_POST['setdevprop']) ){
    if( isset($_POST[$_POST['setdevprop']])){
        if($_POST['setdevprop']=='xrecport')
            modify_device_prop( $device, 'xrecport', explode( " ", $_POST['xrecport'] ));
        else
            modify_device_prop($device,$_POST['setdevprop'],$_POST[$_POST['setdevprop']]);
    }
    if( in_array($_POST['setdevprop'],$presetkeys) )
        modify_device_prop($device,'preset','');
}
if( isset($_POST['setdevpropfloat']) ){
    if( isset($_POST[$_POST['setdevpropfloat']]))
        modify_device_prop($device,$_POST['setdevpropfloat'],floatval($_POST[$_POST['setdevpropfloat']]));
    if( in_array($_POST['setdevpropfloat'],$presetkeys) )
        modify_device_prop($device,'preset','');
}
if( isset($_POST['setdevpropbool']) ){
    if( isset($_POST[$_POST['setdevpropbool']]))
        modify_device_prop($device,$_POST['setdevpropbool'],$_POST[$_POST['setdevpropbool']]=='true');
    if( in_array($_POST['setdevpropbool'],$presetkeys) )
    modify_device_prop($device,'preset','');
}
if( isset($_POST['jsinputchannels']) ){
    modify_device_prop($device,'inputchannels',json_decode($_POST['jsinputchannels']));
    modify_device_prop($device,'preset','');
}
if( isset($_POST['jsfrontendconfig']) )
    modify_device_prop($device,'frontendconfig',json_decode($_POST['jsfrontendconfig']));
if( isset($_POST['devreset']) ){
    if( $dprop['owner'] = $user ){
        rm_device( $device );
        modify_device_prop( $device, 'owner', $user);
    }
}
if( isset($_POST['unclaimdevice']) ){
    if( $devprop['owner'] = $user )
        rm_device( $device );
}
if( isset($_POST['wifi']) && isset($_POST['wifissid'])  && isset($_POST['wifipasswd']) ){
    $devprop = get_properties($device,'device');
    $devprop['wifi'] = $_POST['wifi'] == 'true';
    $devprop['wifissid'] = $_POST['wifissid'];
    $devprop['wifipasswd'] = $_POST['wifipasswd'];
    $devprop['wifiupdate'] = true;
    set_properties( $device, 'device', $devprop );
}

if( isset($_GET['getsessionstat']) ){
    $roomdevs = array();
    if(!empty($dprop['room']))
        $roomdevs = get_devices_in_room( $dprop['room'], false, true);
    $stats = array();
    $names = array();
    $chairs = array();
    $versions = array();
    $fragsize = array();
    $p2p = array();
    $rprop = get_properties($dprop['room'],'room');
    $nullstat = array('lost'=>0,'mean'=>-1,'median'=>-1,'min'=>-1,'p99'=>-1,'received'=>0);
    foreach( $roomdevs as $chair=>$rdprop ){
        $dev = $rdprop['id'];
        //$rdprop = get_properties($dev,'device');
        $pingstat = get_properties($dev.'_'.$dprop['room'],'pingstats');
        unset($pingstat['now']);
        if( empty($pingstat) ){
            // try to get value from room:
            foreach( $roomdevs as $schair=>$srdprop ){
                if( isset($rprop['lat'][$schair.'-'.$chair]) ){
                    $pingstat[$schair] = array('loc'=>$nullstat,'p2p'=>$nullstat,'srv'=>$nullstat,'cur'=>$nullstat);
                    $pingstat[$schair]['packages'] = array('lost'=>0,'received'=>0,'seqerr'=>0,'seqrecovered'=>0);
                    $tmp = $rprop['lat'][$schair.'-'.$chair];
                    if( isset($tmp) ){
                        $pingstat[$schair]['p2p']['median'] = floatval($tmp['lat']);
                        $pingstat[$schair]['p2p']['min'] = floatval($tmp['lat']-0.5*$tmp['jit']);
                        $pingstat[$schair]['p2p']['p99'] = floatval($tmp['lat']+0.5*$tmp['jit']);
                    }
                    if( isset($rprop['lat'][$schair.'-200']) && isset($rprop['lat']['200-'.$chair]) ){
                        $tmp1 = $rprop['lat'][$schair.'-200'];
                        $tmp2 = $rprop['lat']['200-'.$chair];
                        $pingstat[$schair]['srv']['median'] = floatval($tmp1['lat']+$tmp2['lat']);
                        $pingstat[$schair]['srv']['min'] = floatval($tmp1['lat']+$tmp2['lat']-0.5*($tmp1['jit']-$tmp2['jit']));
                        $pingstat[$schair]['srv']['p99'] = floatval($tmp1['lat']+$tmp2['lat']+0.5*($tmp1['jit']-$tmp2['jit']));
                    }
                }
            }
        }
        foreach( $roomdevs as $schair=>$srdprop ){
            if( !isset($pingstat[$schair]) ){
                $pingstat[$schair] = array('loc'=>$nullstat,'p2p'=>$nullstat,'srv'=>$nullstat,'cur'=>$nullstat);
            }
            if( $srdprop['peer2peer'] && $rdprop['peer2peer'] )
                $pingstat[$schair]['cur'] = $pingstat[$schair]['p2p'];
            else
                $pingstat[$schair]['cur'] = $pingstat[$schair]['srv'];
        }
        $stats[$dev] = $pingstat;
        $names[$dev] = $rdprop['label'];
        $chairs[$chair] = $dev;
        $versions[$dev] = intval(version_compare($rdprop['version'],'ovclient-0.6.179-8f84f14'));
        $p2p[$dev] = boolval($rdprop['peer2peer']);
        $fragsize[$dev] = floatval(1000.0*$rdprop['jackperiod']/$rdprop['jackrate']);
    }
    echo(json_encode(array('room'=>$rprop['label'],'names'=>$names,'stats'=>$stats,'chairs'=>$chairs,'versions'=>$versions,
                           'fragsize'=>$fragsize,'n'=>count($roomdevs),'p2p'=>$p2p)));
}

?>
