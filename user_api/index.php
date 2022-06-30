<?php

include '../php/ovbox.inc';
{
    $sitecfg = get_properties('site','config');
    if( $sitecfg['forcehttps'] ){
        if( !(isset($_SERVER['HTTPS']) && $_SERVER['HTTPS'] === 'on') ){
            if( substr_compare( $_SERVER['HTTP_HOST'], 'localhost', 0, 9 )!= 0){
                $actual_link = "https://$_SERVER[HTTP_HOST]$_SERVER[REQUEST_URI]";
                header( "Location: ".$actual_link );
                die();
            }
        }
    }
}

include '../php/rest.inc';
include '../php/user.inc';
include '../php/session.inc';

// now it is safe to do what we want:
flock($fp_dev, LOCK_EX );

if( isset($_GET['mypwreset']) ){
    modify_user_prop( $user, 'validpw', false);
}

if( isset($_GET['devselect']) ){
    select_userdev( $user, $_GET['devselect'] );
    header( "Location: /" );
    die();
}

if((!empty($device)) && ($user != $devprop['owner'])){
    select_userdev( $user, '' );
    header( "Location: /" );
    die();
}

if( isset($_POST['editbulletinboard']) && isset($_POST['bulletinboard']) ){
    $room = $_POST['editbulletinboard'];
    modify_room_prop( $room, 'bulletinboard',$_POST['bulletinboard']);
    header( "Location: /" );
    die();
}

if( isset($_POST['claimdevid']) ){
    $msg = '';
    if( claim_device_id( $user, $_POST['claimdevid'], $msg ) ){
        header( "Location: /" );
    }else{
        print_head( $user, $style, $urlgroup );
        echo '<div class="deverror">'.$msg.'</div>';
        $alink = 'https://' . $_SERVER['HTTP_HOST'];
        if( substr_compare( $_SERVER['HTTP_HOST'], 'localhost', 0, 9 )== 0)
            $alink = 'http://' . $_SERVER['HTTP_HOST'];
        echo '<p><a href="'.$alink.'">Continue</a></p>' . "\n";
        print_foot($style);
    }
    die();
}

if( isset($_GET['setmaingroup']) ){
    if( empty($_GET['setmaingroup']) || in_array($_GET['setmaingroup'],list_groups($user)) ){
        modify_user_prop( $user, 'maingroup', $_GET['setmaingroup'] );
    }
    header( "Location: /" );
}

if( isset($_GET['enterroom']) ) {
    if( !empty( $device ) )
        device_enter_room( $device, $_GET['enterroom'] );
    header( "Location: /" );
    die();
}

if( isset($_GET['swapdev']) ){
    if( !empty( $device ) ){
        room_swap_devices( $device, $_GET['swapdev'] );
    }
    header( "Location: /" );
    die();
}

if( isset($_GET['lockroom']) ){
    if( !empty( $device ) ){
        lock_room( $_GET['lockroom'], $device, $_GET['lck'] );
    }
    header( "Location: /" );
    die();
}

if( isset($_GET['kick']) ){
    $rdev = $_GET['kick'];
    if( !empty($rdev) ){
        $rdevprop = get_properties( $rdev, 'device' );
        if( !empty($rdevprop['room']) ){
            $rprop = get_properties( $rdevprop['room'], 'room' );
            if( ($rprop['owner'] == $user) || ($rdevprop['owner'] == $user) ){
                modify_device_prop( $rdev, 'room', '');
                set_dev_room_pos( $rdevprop['room'] );
            }
        }
    }
    header( "Location: /" );
    die();
}

if( isset($_POST['setdevprop']) ){
    $device = $_POST['setdevprop'];
    if( !empty( $device ) ){
        $prop = get_properties( $device, 'device' );
        $prop['reverb'] = isset($_POST['reverb']);
        $prop['renderism'] = isset($_POST['renderism']);
        $prop['peer2peer'] = isset($_POST['peer2peer']);
        $prop['jackplugdev'] = isset($_POST['jackplugdev']);
        $prop['rawmode'] = isset($_POST['rawmode']);
        $prop['virtualacoustics'] = isset($_POST['virtualacoustics']);
        $prop['selfmonitor'] = isset($_POST['selfmonitor']);
        $prop['sendlocal'] = isset($_POST['sendlocal']);
        $prop['headtracking'] = isset($_POST['headtracking']);
        $prop['headtrackingrot'] = isset($_POST['headtrackingrot']);
        $prop['headtrackingrotsrc'] = isset($_POST['headtrackingrotsrc']);
        $prop['message'] = '';
        set_getprop_post_float($prop,'jittersend');
        set_getprop_post_float($prop,'jitterreceive');
        set_getprop_post($prop,'label');
        set_getprop_post_float($prop,'egogain');
        set_getprop_post_float($prop,'selfmonitordelay');
        set_getprop_post_float($prop,'rvbgain');
        if( isset($_POST['jsinputchannels']) ){
            $prop['inputchannels'] = json_decode($_POST['jsinputchannels']);
        }
        set_getprop_post($prop,'srcshiftxyz');
        set_getprop_post($prop,'outputport1');
        set_getprop_post($prop,'outputport2');
        set_getprop_post($prop,'xport');
        set_getprop_post_float($prop,'secrec');
        set_getprop_post_float($prop,'playbackgain');
        set_getprop_post_float($prop,'mastergain');
        set_getprop_post($prop,'rectype');
        set_getprop_post($prop,'jackdevice');
        set_getprop_post_float($prop,'jackrate');
        set_getprop_post_float($prop,'jackperiod');
        set_getprop_post_float($prop,'jackbuffers');
        set_getprop_post_float($prop,'headtrackingport');
        set_getprop_post_float($prop,'headtrackingtauref');
        if( isset($_POST['xrecport']) )
            $prop['xrecport'] = explode( " ", $_POST['xrecport'] );
        if( isset($_POST['jsfrontendconfig']) )
            $prop['frontendconfig'] = json_decode($_POST['jsfrontendconfig']);
        $prop['preset'] = '';
        set_properties( $device, 'device', $prop );
    }
    header( "Location: /" );
    die();
}

if( isset($_GET['resetroom']) ){
    $room = $_GET['resetroom'];
    $rprop = get_room_prop($room);
    if( ($user == $rprop['owner']) || ($rprop['editable'])){
        unset($rprop['rvbgain']);
        unset($rprop['rvbabs']);
        unset($rprop['rvbdamp']);
        unset($rprop['size']);
        set_properties( $room, 'room', $rprop );
    }
    header( "Location: /" );
    die();
}

if( isset($_POST['setroom']) ){
    $room = $_POST['setroom'];
    $rprop = get_room_prop($room);
    if( $user == $rprop['owner']){
        $rprop['editable'] = isset($_POST['editable']);
        $rprop['private'] = isset($_POST['private']);
        set_getprop_post( $rprop, 'group' );
    }
    if( ($user == $rprop['owner']) || ($rprop['editable'])){
        if( isset($_POST['label']))
            $rprop['label'] = $_POST['label'];
        set_getprop_post( $rprop, 'size' );
        if( isset($_POST['sx'])&&isset($_POST['sy'])&&isset($_POST['sz']))
            $rprop['size'] = $_POST['sx'].' '.$_POST['sy'].' '.$_POST['sz'];
        set_getprop_post( $rprop, 'rvbgain' );
        set_getprop_post( $rprop, 'rvbdamp' );
        set_getprop_post( $rprop, 'rvbabs' );
        set_getprop_post( $rprop, 'ambientsound' );
        set_getprop_post( $rprop, 'ambientlevel' );
        set_properties( $room, 'room', $rprop );
    }
    header( "Location: /" );
    die();
}

if( isset($_GET['clearroom']) ){
    $room = $_GET['clearroom'];
    $rprop = get_room_prop($room);
    if( $user == $rprop['owner']){
        $roomdev = get_devices_in_room( $room );
        foreach( $roomdev as $dev ){
            modify_device_prop( $dev, 'room', '');
        }
    }
    header( "Location: /" );
    die();
}

if( isset($_GET['claim']) ){
    $devs = list_unclaimed_devices();
    if( in_array( $_GET['claim'], $devs ) ){
        modify_device_prop( $_GET['claim'], 'owner', $user );
        select_userdev($user, $_GET['claim']);
    }
    header( "Location: /" );
    die();
}

if ( empty( $device ) ) {
    foreach( owned_devices($user) as $dev=>$dprop ){
        header( "Location: /?devselect=" . $dev );
        die();
    }
}

$msg = '';
if( isset($_POST['usermail']) )
    modify_user_prop( $user, 'mail', $_POST['usermail'] );
if( isset($_POST['updatepassword']) )
    update_pw( $_POST['password'], $user, $msg );


print_head( $user, $style, $urlgroup );

if( !empty($msg) ){
    echo '<div class="deverror">'.$msg.'</div>';
}
echo '<div><span class="ovtitle">Session</span><div class="help">Need help? - <a target="blank" href="https://github.com/gisogrimm/ovbox/wiki">Wiki-Pages</a> / <a target="blank" href="https://forum.digital-stage.org/">DS-Forum</a></div></div>';

echo '<div class="devclaim" id="devclaim" style="display:none;"></div>';

html_show_device( $user, $device, $devprop );

echo '<div id="roomlist" claas="roomlist"><span id="roomlistremove">loading room list...<span></div>';

print_foot($style);

?>
