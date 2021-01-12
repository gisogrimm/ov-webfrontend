<?php

if( !(isset($_SERVER['HTTPS']) && $_SERVER['HTTPS'] === 'on') ){
    if( substr_compare( $_SERVER['HTTP_HOST'], 'localhost', 0, 9 )!= 0){
        $actual_link = "https://$_SERVER[HTTP_HOST]$_SERVER[REQUEST_URI]";
        header( "Location: ".$actual_link );
        die();
    }
}

include '../php/ovbox.inc';

if( isset($_GET['activate']) ){
    flock($fp_user, LOCK_EX );
    $urlgroup = '';
    if( isset($_GET['grp'] ) )
        $urlgroup = get_group_by_hash($_GET['grp']);
    $style = '';
    if( !empty($urlgroup) ){
        $gprop = get_properties( $urlgroup, 'group' );
        $style = $gprop['style'];
    }
    unset($_SESSION['user']);
    session_unset();
    session_destroy();
    print_head('',$style);
    echo '<div style="padding: 20px; background-color: #ffffff70;margin: 8px;">';
    if( activate_new_user($_GET['activate']) ){
        echo '<h2>Account activated</h2>';
        echo '<p>Now you can login to the ovbox configuration page.</p>';
    }else{
        echo '<h2>Activation failed</h2>';
        echo '<p>Sorry, your account activation failed. The link was invalid or expired. Please retry to register.</p>';
    }
    echo '<p><a href="?grp='.grouphash($urlgroup).'">Login page</a></p>';
    echo '</div>';
    print_foot( $style );
    die();
}

session_start();
if( isset($_GET['pwreset']) ){
    flock($fp_user, LOCK_EX );
    $urlgroup = '';
    if( isset($_GET['grp'] ) )
        $urlgroup = get_group_by_hash($_GET['grp']);
    $style = '';
    if( !empty($urlgroup) ){
        $gprop = get_properties( $urlgroup, 'group' );
        $style = $gprop['style'];
    }
    $puser = '';
    if( validate_pwreset($_GET['pwreset'],$puser) ){
        $_SESSION['user'] = $puser;
    }else{
        unset($_SESSION['user']);
        session_unset();
        session_destroy();
        header( "Location: ?grp=".grouphash($urlgroup)."&fail=" );
        die();
    }
}
$urlgroup = '';
if( isset($_GET['grp'] ) )
    $urlgroup = get_group_by_hash($_GET['grp']);
$style = '';
if( !empty($urlgroup) ){
    $gprop = get_properties( $urlgroup, 'group' );
    $style = $gprop['style'];
}

if( !isset($_SESSION['user']) ){
    if( isset($_POST['forgotpw']) ){
        // we are registering a new user:
        print_head('',$style);
        echo '<div style="padding: 20px; background-color: #ffffff70;margin: 8px;">';
        $msg = '';
        flock($fp_user, LOCK_EX );
        flock($fp_register, LOCK_EX );
        $block = true;
        request_passwd_reset( $_POST['pwrusername'] );
        echo '<h2>Password reset:</h2>';
        echo '<p>If you entered a valid user name and an e-mail address was stored
in the user profile, a password reset link has been sent to your email
address. Please follow that link within one hour to reset your
password. If you did not receive a message, please check in your spam
folder. Maybe your account was deleted due to inactivity - then please create a new account.</p>';
        echo '<p><a href="/">Back to login page</a></p>';
        echo '</div>';
        flock($fp_user, LOCK_UN );
        print_foot( $style );
        die();
    }
    if( !(isset($_POST['login']) || isset($_POST['register'])) ) {
        // neither login nor register was clicked before, thus display login page:
        print_head('',$style);
        // display login box: username, password
        echo '<div style="padding: 20px; background-color: #ffffff70;margin: 8px;">';
        if( isset($_GET['fail']) )
            echo '<div class="failure">Sorry, invalid user name or password.</div>';
        echo '<h2>Login:</h2>';
        echo '<form class="login" method="POST">';
        echo '<label>User name:</label><br>';
        echo '<input type="text" name="username"><br>';
        echo '<label>Password:</label><br>';
        echo '<input type="password" name="password"><br>';
        echo '<input type="hidden" name="login">';
        echo '<button>Login</button>';
        echo '</form>';
        echo '<p><a href="register.php?grp='.grouphash($urlgroup).'">Register as new user</a> &nbsp; '.
            '<a href="forgotpw.php?grp='.grouphash($urlgroup).'">I forgot my password</a></p>';
        echo '</div>';
        print_foot( $style, false );
        die();
    }
    if( isset($_POST['register']) ){
        // we are registering a new user:
        print_head('',$style);
        echo '<div style="padding: 20px; background-color: #ffffff70;margin: 8px;">';
        $msg = '';
        flock($fp_user, LOCK_EX );
        flock($fp_register, LOCK_EX );
        $block = true;
        if( register_new_user( $_POST['mail'], $_POST['username'], $_POST['password'], $urlgroup, $msg ) ){
            echo '<h2>Account activation:</h2>';
            echo '<p>An activation link has been sent to your email address. Please follow that link within one hour to activate your account. If you did not receive a message, please check in your spam folder.</p>';
            echo '<p><a href="/">Back to login page</a></p>';
            $block = true;
        }else{
            echo '<p>'.$msg.'</p>';
            echo '<p><a href="register.php?grp='.grouphash($urlgroup).'">Back to registration page</a></p>';
        }
        echo '</div>';
        flock($fp_user, LOCK_UN );
        print_foot( $style );
        flush();
        ob_flush();
        if( $block )
            sleep( 5 );
        die();
    }
    // if we are here this means login data was entered:
    flock($fp_authfail, LOCK_EX );
    if( !auth($_POST['username'], $_POST['password']) ) {
        flock($fp_user, LOCK_UN );
        sleep( 1 );
        session_abort();
        header( "Location: ?grp=".grouphash($urlgroup)."&fail=" );
        die();
    }
    flock($fp_authfail, LOCK_UN );
    // login was successful!
    session_regenerate_id();
    // auth okay, setup session
    $_SESSION['user'] = $_POST['username'];
    header( "Location: ?grp=".grouphash($urlgroup) );
    die();
}

// in a current session the logout was clicked - destroy session:
if( isset($_POST['logout']) ){
    unset($_SESSION['user']);
    session_unset();
    session_destroy();
    header( "Location: ?grp=".grouphash($urlgroup) );
    die();
}

$user = $_SESSION['user'];
flock($fp_user, LOCK_EX );
if( !in_array($user,list_users()) ){
    session_unset();
    session_destroy();
    header( "Location: ?grp=".grouphash($urlgroup) );
    die();
}
flock($fp_user, LOCK_UN );

if( isset($_POST['terminateaccount']) ){
    flock($fp_user, LOCK_EX );
    terminate_account( $user );
    session_unset();
    session_destroy();
    header( "Location: ?grp=".grouphash($urlgroup) );
    die();
}

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

if( isset($_GET['firmwareupdate']) ){
    modify_device_prop( $_GET['firmwareupdate'], 'firmwareupdate', true );
    header( "Location: /" );
    die();
}

$device = get_device( $user );
if( !empty($device) ){
    $devprop = get_properties( $device, 'device' );
    if($user != $devprop['owner']){
        select_userdev( $user, '' );
        header( "Location: /" );
        die();
    }
}
$usergroups = list_groups($user);
$userprop = get_properties($user,'user');
$maingroup = $userprop['maingroup'];
if( !in_array($maingroup,$usergroups) )
    $maingroup = '';
$style = '';
if( !empty($maingroup) ){
    $groupprop = get_properties( $maingroup, 'group' );
    $style = $groupprop['style'];
}


if( isset($_POST['claimdevid']) ){
    $msg = '';
    if( claim_device_id( $user, $_POST['claimdevid'], $msg ) ){
        header( "Location: /" );
    }else{
        print_head( $user, $style );
        echo '<div class="deverror">'.$msg.'</div>';
        $alink = 'https://' . $_SERVER['HTTP_HOST'];
        if( substr_compare( $_SERVER['HTTP_HOST'], 'localhost', 0, 9 )== 0)
            $alink = 'http://' . $_SERVER['HTTP_HOST'];
        echo '<p><a href="'.$alink.'">Continue</a></p>' . "\n";
        print_foot($style);
    }
    die();
}

if( isset($_POST['transferownership']) ){
    $device = $_POST['transferownership'];
    flock($fp_user, LOCK_EX );
    $newowner = $_POST['newowner'];
    $users = list_users();
    if( !in_array( $newowner, $users ) ){
        print_head( $user, $style );
        echo '<div class="deverror">The new owner "'.$newowner.'" is not registered as a user. The ownership of device "'.$device.'" was not transferred.</div>';
        $alink = 'https://' . $_SERVER['HTTP_HOST'];
        if( substr_compare( $_SERVER['HTTP_HOST'], 'localhost', 0, 9 )== 0)
            $alink = 'http://' . $_SERVER['HTTP_HOST'];
        echo '<p><a href="'.$alink.'">Continue</a></p>' . "\n";
        print_foot($style);
    }else{
        modify_device_prop( $device, 'owner', $newowner );
        header( "Location: /" );
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
            if( ($rprop['owner'] == $user) || ($rdevprop['owner'] == $user) )
                modify_device_prop( $rdev, 'room', '');
        }
    }
    header( "Location: /" );
    die();
}

function set_getprop( &$prop, $key )
{
    if( isset($_GET[$key]) ){
        $prop[$key] = $_GET[$key];
    }
}

function set_getprop_post( &$prop, $key )
{
    if( isset($_POST[$key]) ){
        $prop[$key] = $_POST[$key];
    }
}

if( isset($_GET['jackrestart']) ){
    $device = $_GET['jackrestart'];
    if( !empty( $device ) ){
        modify_device_prop( $device, 'jackrestart', true );
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
        $prop['donotsend'] = isset($_POST['donotsend']);
        $prop['selfmonitor'] = isset($_POST['selfmonitor']);
        $prop['sendlocal'] = isset($_POST['sendlocal']);
        $prop['headtracking'] = isset($_POST['headtracking']);
        $prop['headtrackingrot'] = isset($_POST['headtrackingrot']);
        $prop['headtrackingrotsrc'] = isset($_POST['headtrackingrotsrc']);
        $prop['message'] = '';
        set_getprop_post($prop,'jittersend');
        set_getprop_post($prop,'jitterreceive');
        set_getprop_post($prop,'label');
        set_getprop_post($prop,'egogain');
        set_getprop_post($prop,'rvbgain');
        if( isset($_POST['jsinputchannels']) ){
            $prop['inputchannels'] = json_decode($_POST['jsinputchannels']);
        }
        set_getprop_post($prop,'srcdist');
        set_getprop_post($prop,'srcshiftxyz');
        set_getprop_post($prop,'outputport1');
        set_getprop_post($prop,'outputport2');
        set_getprop_post($prop,'xport');
        set_getprop_post($prop,'secrec');
        set_getprop_post($prop,'playbackgain');
        set_getprop_post($prop,'mastergain');
        set_getprop_post($prop,'rectype');
        set_getprop_post($prop,'jackdevice');
        set_getprop_post($prop,'jackrate');
        set_getprop_post($prop,'jackperiod');
        set_getprop_post($prop,'jackbuffers');
        set_getprop_post($prop,'headtrackingport');
        if( isset($_POST['xrecport']) )
            $prop['xrecport'] = explode( " ", $_POST['xrecport'] );
        if( isset($_POST['jsfrontendconfig']) )
            $prop['frontendconfig'] = json_decode($_POST['jsfrontendconfig']);
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
    if( in_array( $_GET['claim'], $devs ) )
        modify_device_prop( $_GET['claim'], 'owner', $user );
    header( "Location: /" );
    die();
}

if( isset($_GET['unclaim']) ){
    if( $devprop['owner'] = $user )
        rm_device( $device );
    //modify_device_prop( $device, 'owner', '');
    header( "Location: /" );
    die();
}

if( isset($_GET['devreset']) ){
    if( $devprop['owner'] = $user ){
        rm_device( $device );
        modify_device_prop( $device, 'owner', $user);
        modify_device_prop( $device, 'version', $devprop['version']);
    }
    header( "Location: /" );
    die();
}

if( isset($_POST['contact']) ){
    submit_contact( $user, $_POST['contact'], $_POST['message'] );
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
if( isset($_POST['agreepriv']) )
    modify_user_prop( $user, 'agreedprivacy', true );
if( isset($_POST['agreeterms']) )
    modify_user_prop( $user, 'agreedterms', true );
if( isset($_POST['updatepassword']) )
    update_pw( $_POST['password'], $user, $msg );

// age
modify_user_prop( $user, 'access', time() );

print_head( $user, $style );

// admin area:
$site = get_properties('site','config');
$isadmin = in_array($user,$site['admin']);
if( $isadmin ){
    echo '<p class="adminarea">';
    echo '<a href="/?">Home</a> - admin <a href="admin.php?adm=devices">devices</a> <a href="admin.php?adm=rooms">rooms</a> <a href="admin.php?adm=users">users</a> <a href="admin.php?adm=groups">groups</a>';
    echo '</p>';
}

if( !empty($msg) ){
    echo '<div class="deverror">'.$msg.'</div>';
}
echo '<div class="help">Need help? - <a target="blank" href="https://github.com/gisogrimm/ovbox/wiki">Wiki-Pages</a> / <a target="blank" href="https://forum.digital-stage.org/">DS-Forum</a></div>';
echo '<form class="login" method="POST">';
echo '<input type="hidden" name="logout"><br>';
echo '<button class="uibutton">Logout</button>';
echo '</form>';

$showrooms = true;
// check for privacy/terms acceptance:
$userprop = get_properties( $user, 'user' );
if( !($userprop['agreedterms'] && $userprop['agreedprivacy']) ){
    $doc = new DOMDocument('1.0');
    $form = $doc->appendChild($doc->createElement('form'));
    $form->setAttribute('class','devprop');
    $form->setAttribute('method','POST');
    $inp = $form->appendChild($doc->createElement('input'));
    $inp->setAttribute('name','agreepriv');
    $inp->setAttribute('type','checkbox');
    $inp->setAttribute('required','');
    if( $userprop['agreedprivacy'] )
        $inp->setAttribute('checked','');
    $lab = $form->appendChild($doc->createElement('label'));
    $lab->setAttribute('for','agreepriv');
    $lab->appendChild($doc->createTextNode('I have read and accept the '));
    $a = $lab->appendChild($doc->createElement('a'));
    $a->setAttribute('target','blank');
    $a->setAttribute('href','privacy.php');
    $a->appendChild($doc->createTextNode('privacy policy'));
    $lab->appendChild($doc->createTextNode(' / '));
    $a = $lab->appendChild($doc->createElement('a'));
    $a->setAttribute('target','blank');
    $a->setAttribute('href','privacy.php');
    $a->appendChild($doc->createTextNode('Datenschutzerkärung'));
    $lab->appendChild($doc->createTextNode('.'));
    $form->appendChild($doc->createElement('br'));
    // terms:
    $inp = $form->appendChild($doc->createElement('input'));
    $inp->setAttribute('name','agreeterms');
    $inp->setAttribute('type','checkbox');
    $inp->setAttribute('required','');
    if( $userprop['agreedterms'] )
        $inp->setAttribute('checked','');
    $lab = $form->appendChild($doc->createElement('label'));
    $lab->setAttribute('for','agreeterms');
    $lab->appendChild($doc->createTextNode('I have read and accept the '));
    $a = $lab->appendChild($doc->createElement('a'));
    $a->setAttribute('target','blank');
    $a->setAttribute('href','terms.php');
    $a->appendChild($doc->createTextNode('Terms of Service'));
    $lab->appendChild($doc->createTextNode('.'));
    $form->appendChild($doc->createElement('br'));
    //echo '<input type="checkbox" name="agreeterms" required><label for="agreeterms">I have read and accept the <a target="blank" href="terms.php">terms of service</a>.</label></span><br>';
    //echo '<input type="hidden" name="register">';
    //echo '<input type="submit" value="Register">';
    $inp = $form->appendChild($doc->createElement('input'));
    $inp->setAttribute('type','submit');
    $inp->setAttribute('value','Submit');
    echo $doc->saveHTML();
    $showrooms = false;
}

if( !$userprop['validpw'] ){
    $doc = new DOMDocument('1.0');
    $form = $doc->appendChild($doc->createElement('form'));
    $form->setAttribute('class','devprop');
    $form->setAttribute('method','POST');
    $form->setAttribute('action','/?grp='.grouphash($urlgroup));
    $lab = $form->appendChild($doc->createElement('label'));
    $lab->appendChild($doc->createTextNode('New password (minimum 6 characters, 1 letter, 1 number): '));
    $form->appendChild($doc->createElement('br'));
//echo '<input type="password" name="password"><br>';
    $inp = $form->appendChild($doc->createElement('input'));
    $inp->setAttribute('name','password');
    $inp->setAttribute('type','password');
    $inp->setAttribute('required','');
    $inp = $form->appendChild($doc->createElement('input'));
    $inp->setAttribute('name','updatepassword');
    $inp->setAttribute('type','hidden');
    $inp = $form->appendChild($doc->createElement('input'));
    $inp->setAttribute('type','submit');
    $inp->setAttribute('value','Submit');
    echo $doc->saveHTML();
    $showrooms = false;
}

if( $showrooms ){
    echo '<div class="devclaim" id="devclaim" style="display:none;"></div>';
    if ( empty( $device ) ) {
        $devprop = defaults('device');
    } else {
        $devprop = get_properties( $device, 'device' );
    }
    html_show_user( $user, $device, $devprop );
    if( !empty($device) )
        html_show_device( $user, $device, $devprop );
    echo '<div id="roomlist" claas="roomlist"><span id="roomlistremove">loading room list...<span></div>';
}

$doc = new DOMDocument('1.0');
$root = $doc->appendChild($doc->createElement('div'));
$inp = $root->appendChild($doc->createElement('input'));
$inp->setAttribute('type','button');
$inp->setAttribute('class','uibutton');
$inp->setAttribute('onclick','toggledisplay(\'accterm\',\'account termination options\');');
$inp->setAttribute('value','show account termination options');
$form = $root->appendChild($doc->createElement('form'));
$form->setAttribute('method','POST');
$form->setAttribute('class','devprop');
$form->setAttribute('id','accterm');
$form->setAttribute('style','display: none;');
$el = $form->appendChild($doc->createElement('input'));
$el->setAttribute('name','terminateaccount');
$el->setAttribute('id','terminateaccount');
$el->setAttribute('type','checkbox');
$el = $form->appendChild($doc->createElement('label'));
$el->setAttribute('for','terminateaccount');
$el->appendChild($doc->createTextNode('I understand that the termination of my account cannot be undone. It will also remove all device configurations linked to this account.'));
$form->appendChild($doc->createElement('br'));
$el = $form->appendChild($doc->createElement('button'));
$el->appendChild($doc->createTextNode('terminate account'));

$root->appendChild($doc->createElement('br'));
$inp = $root->appendChild($doc->createElement('input'));
$inp->setAttribute('type','button');
$inp->setAttribute('class','uibutton');
$inp->setAttribute('onclick','toggledisplay(\'contactform\',\'contact form\');');
$inp->setAttribute('value','show contact form');
$form = $root->appendChild($doc->createElement('form'));
$form->setAttribute('method','POST');
$form->setAttribute('class','devprop');
$form->setAttribute('id','contactform');
$form->setAttribute('style','display: none;');
$el = $form->appendChild($doc->createElement('label'));
$el->setAttribute('for','contact');
$el->appendChild($doc->createTextNode('Enter your comments or wishes here:'));
$form->appendChild($doc->createElement('br'));
$el = $form->appendChild($doc->createElement('textarea'));
$el->setAttribute('rows','5');
$el->setAttribute('cols','60');
$el->setAttribute('name','message');
$el->setAttribute('required','');
$form->appendChild($doc->createElement('br'));
$el = $form->appendChild($doc->createElement('label'));
$el->setAttribute('for','contact');
$el->appendChild($doc->createTextNode('If you would like to receive an answer, please indicate here how we can contact you (e.g. your e-mail address):'));
$form->appendChild($doc->createElement('br'));
$el = $form->appendChild($doc->createElement('input'));
$el->setAttribute('type','text');
$el->setAttribute('name','contact');
$el->setAttribute('value',$userprop['mail']);
$form->appendChild($doc->createElement('br'));
$el = $form->appendChild($doc->createElement('button'));
$el->appendChild($doc->createTextNode('Submit'));

echo $doc->saveHTML();


print_foot($style);

?>
