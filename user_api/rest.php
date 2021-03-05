<?php

if( !(isset($_SERVER['HTTPS']) && $_SERVER['HTTPS'] === 'on') ){
    if( substr_compare( $_SERVER['HTTP_HOST'], 'localhost', 0, 9 )!= 0){
        $actual_link = "https://$_SERVER[HTTP_HOST]$_SERVER[REQUEST_URI]";
        header( "Location: ".$actual_link );
        die();
    }
}

include '../php/ovbox.inc';

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
    die();
}
if( isset($_GET['devpresetsave']) ){
    if( !empty($_GET['devpresetsave']) ){
        $presets = get_properties( $user, 'devpresets' );
        $preset = array();
        foreach( array('selfmonitor',
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
                       'jackdevice',
                       'jackplugdev',
                       'jackrate',
                       'jackperiod',
                       'jackbuffers') as $key )
            $preset[$key] = $dprop[$key];
        $presets[$_GET['devpresetsave']] = $preset;
        set_properties( $user, 'devpresets', $presets );
        modify_device_prop($device,'preset',$_GET['devpresetsave']);
    }
}
if( isset($_GET['devpresetload']) ){
    $presets = get_properties( $user, 'devpresets' );
    if( array_key_exists( $_GET['devpresetload'], $presets ) ){
        $preset = $presets[$_GET['devpresetload']];
        foreach( $preset as $key=>$value )
            $dprop[$key]=$value;
        set_properties($device,'device',$dprop);
        modify_device_prop($device,'preset',$_GET['devpresetload']);
    }
}
if( isset($_GET['devpresetrm']) ){
    $presets = get_properties( $user, 'devpresets' );
    if( array_key_exists( $_GET['devpresetrm'], $presets ) ){
        unset( $presets[$_GET['devpresetrm']]);
        set_properties( $user, 'devpresets', $presets );
        if( $dprop['preset'] == $_GET['devpresetsave'] )
            modify_device_prop($device,'preset','');
    }
}
?>
