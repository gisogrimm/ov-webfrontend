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
?>
