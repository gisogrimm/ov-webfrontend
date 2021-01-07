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

$site = get_properties('site','config');
if( !in_array($user,$site['admin']) ){
    unset($_SESSION['user']);
    session_unset();
    session_destroy();
    header( "Location: /" );
    die();
}

// get exclusive lock on database and users:
flock($fp_dev, LOCK_EX );
flock($fp_user, LOCK_EX );
if( isset($_GET['adminterminateaccount']) ){
    terminate_account( $_GET['adminterminateaccount'] );
    header( "Location: /admin.php?adm=users" );
    die();
}
if( isset($_GET['addgroup']) ){
    add_group($_GET['addgroup']);
    header( "Location: /admin.php?adm=groups" );
    die();
}
if( isset($_GET['rmgroup']) ){
    rm_group($_GET['rmgroup']);
    header( "Location: /admin.php?adm=groups" );
    die();
}
if( isset($_GET['addusertogroup']) ){
    add_user_to_group($_GET['newuser'],$_GET['addusertogroup']);
    header( "Location: /admin.php?adm=groups" );
    die();
}
if( isset($_GET['removeuserfromgroup']) ){
    remove_user_from_group($_GET['groupuser'],$_GET['removeuserfromgroup']);
    header( "Location: /admin.php?adm=groups" );
    die();
}
if( isset($_GET['rmoldrooms'])){
    rm_old_rooms();
    header( "Location: /admin.php?adm=rooms" );
    die();
}
if( isset($_GET['rmolddevs'])){
    rm_old_unclaimed_devices();
    header( "Location: /admin.php?adm=devices" );
    die();
}
if( isset($_GET['setgrpstyle'])){
    modify_group_prop( $_GET['setgrpstyle'], 'style', $_GET['grpstyle']);
    header( "Location: /admin.php?adm=groups" );
    die();
}
if( isset($_GET['moduser']) ){
    modify_user_prop( $_GET['moduser'], 'seesall', isset($_GET['seesall']));
    modify_user_prop( $_GET['moduser'], 'maingroup', $_GET['maingroup']);
    if( !in_array( $_GET['maingroup'], list_groups($_GET['moduser']))){
        add_user_to_group($_GET['moduser'],$_GET['maingroup']);
    }
    header( "Location: /admin.php?adm=users" );
    die();
}
if( isset($_GET['sortby']) ){
    $key = $_GET['sortby'];
    $cat = $_GET['category'];
    $prop = get_properties( 'list', 'sortkey' );
    $prop[$cat] = $key;
    set_properties('list','sortkey', $prop );
    header( "Location: /admin.php?adm=" . $cat . "s" );
    die();
}
if( isset($_GET['setdeviceowner']) ){
    modify_device_prop( $_GET['setdeviceowner'], 'owner', $_GET['owner'] );
    header( "Location: /admin.php?adm=devices" );
    die();
}
if( isset($_GET['setdevicelabel']) ){
    modify_device_prop( $_GET['setdevicelabel'], 'label', $_GET['label'] );
    header( "Location: /admin.php?adm=devices" );
    die();
}
if( isset($_GET['rmdevice']) ){
    rm_device( $_GET['rmdevice'] );
    header( "Location: /admin.php?adm=devices" );
    die();
}
if( isset($_GET['setroomowner']) ){
    modify_room_prop( $_GET['setroomowner'], 'owner', $_GET['owner'] );
    header( "Location: /admin.php?adm=rooms" );
    die();
}
if( isset($_GET['setroomgroup']) ){
    modify_room_prop( $_GET['setroomgroup'], 'group', $_GET['group'] );
    header( "Location: /admin.php?adm=rooms" );
    die();
}
if( isset($_GET['setroomlabel']) ){
    modify_room_prop( $_GET['setroomlabel'], 'label', $_GET['label'] );
    header( "Location: /admin.php?adm=rooms" );
    die();
}
if( isset($_GET['rmroom']) ){
    rm_room( $_GET['rmroom'] );
    header( "Location: /admin.php?adm=rooms" );
    die();
}
print_head( $user );
rm_old_unclaimed_devices();
$adm = 'devices';
if( isset($_GET['adm']) ){
    $adm = $_GET['adm'];
}
echo '<p class="adminarea">';
echo '<a href="/?">Home</a> - admin <a href="admin.php?adm=devices">devices</a> <a href="admin.php?adm=rooms">rooms</a> <a href="admin.php?adm=users">users</a> <a href="admin.php?adm=groups">groups</a>';
echo ' - <input type="button" onclick="location.replace(\'/admin.php?adm='.$adm.'\');" value="Refresh"/>'.
    ' <span class="timedisplay">0</span>'."\n";
    echo '</p>';
if( $adm == 'devices' ){
    html_admin_db('device',array('roomage','version'));
    echo '<form><input type="hidden" name="rmolddevs"/><button>Remove inactive unclaimed devices</button></form>';
}
if( $adm == 'rooms' ){
    html_admin_rooms();
    echo '<form><input type="hidden" name="rmoldrooms"/><button>Remove inactive unclaimed rooms</button></form>';
}
if( $adm == 'users' ){
    html_admin_users();
}
if( $adm == 'groups' ){
    html_admin_groups();
}
flock($fp_dev, LOCK_UN );
flock($fp_user, LOCK_UN );
echo '<p> </p>';
print_foot('',false);

?>
