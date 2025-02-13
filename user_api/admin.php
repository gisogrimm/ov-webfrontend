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

include '../php/admin.inc';
include '../php/user.inc';
include '../php/session.inc';

if( !in_array($user,$site['admin']) ){
    unset($_SESSION['user']);
    session_unset();
    session_destroy();
    header( "Location: /" );
    die();
}

$subscriptionadmin = in_array($user,$site['subscriptionadmin']);

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
print_head( $user, $style, $urlgroup );

function print_val_class( $v, $t1, $t2, $c0, $c1, $c2, $decimals, $unit ){
    if( $v < $t1 )
        echo '<span class="'.$c0.'">';
    else if( $v < $t2 )
        echo '<span class="'.$c1.'">';
    else
        echo '<span class="'.$c2.'">';
    echo number_format($v, $decimals ).$unit.'</span>';
}

$loadavg = sys_getloadavg();
$cpuload = getServerLoad();
$diskspace = disk_free_space('.')/1000000000;
if( $loadavg || $cpuload ){
    echo '<p>Server CPU load: ';
    print_val_class( $cpuload, 10, 50, 'cputempgood','cputempwarn','cputempcritical',1,'%');
    echo '<br/>Load average: ';
    print_val_class( $loadavg[0], 1, 2, 'cputempgood','cputempwarn','cputempcritical',2,'');
    echo '/';
    print_val_class( $loadavg[1], 1, 2, 'cputempgood','cputempwarn','cputempcritical',2,'');
    echo '/';
    print_val_class( $loadavg[2], 1, 2, 'cputempgood','cputempwarn','cputempcritical',2,'');
    echo '<br/>Disk space: ';
    print_val_class( $diskspace, 1, 2, 'cputempcritical','cputempwarn','cputempgood',2,' GB');
    echo '</p>';
}

rm_old_unclaimed_devices();
$adm = 'devices';
if( isset($_GET['adm']) ){
    $adm = $_GET['adm'];
}

if( $adm == 'devices' ){
    html_admin_db('device',array('version','uname_machine','uname_sysname','uname_release'));
    echo '<form><input type="hidden" name="rmolddevs"/><button>Remove inactive unclaimed devices</button></form>';
}
if( $adm == 'rooms' ){
    html_admin_rooms();
}
if( $adm == 'users' ){
    html_admin_users();
}
if( $adm == 'groups' ){
    html_admin_groups();
}
if( $adm == 'edituser' ){
    $usr = '';
    if( isset($_GET['admusr']) ){
        $usr = $_GET['admusr'];
        html_admin_edituser($usr, $site);
    }
}
flock($fp_dev, LOCK_UN );
flock($fp_user, LOCK_UN );
echo '<p> </p>';
print_foot('',false);

?>
