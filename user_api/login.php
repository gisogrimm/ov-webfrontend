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

$gprop = array('preamble'=>'<div>The <em>ovbox</em> is a remote music collaboration system developed by
the ORLANDOviols ensemble during the Covid19 pandemic.</div>'."\n");

// style settings:
$urlgroup = '';
if( isset($_GET['grp'] ) )
    $urlgroup = get_group_by_hash($_GET['grp']);
$style = '';

if( !empty($urlgroup) ){
    $gprop = get_properties( $urlgroup, 'group' );
    $style = $gprop['style'];
}

if( isset($_GET['activate']) ){
    flock($fp_user, LOCK_EX );
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

$site = get_properties('site','config');

if( !isset($_SESSION['user']) ){
    if( isset($_POST['forgotpw']) ){
        // we are registering a new user:
        print_head('',$style,$urlgroup);
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
        print_head('',$style,$urlgroup);
        // display login box: username, password
        echo '<div style="padding: 20px; background-color: #ffffff70;margin: 8px;">';
        echo $gprop['preamble'];
        if( isset($_GET['fail']) )
            echo '<div class="failure">Sorry, invalid user name or password.</div>';
        $dispuser = '';
        $focuser = 'autofocus';
        $focpw = '';
        if( isset($_GET['usr']) ){
            $dispuser = $_GET['usr'];
            $focuser = '';
            $focpw = 'autofocus';
        }
        echo '<h2>Login:</h2>';
        echo '<form class="login" method="POST">';
        echo '<div class="loginlab">User name:</div>';
        echo '<input class="logininp" type="text" name="username" value="'.$dispuser.'" '.$focuser.'><br>';
        echo '<div class="loginlab">Password:</div>';
        echo '<input class="logininp" type="password" name="password" '.$focpw.'><br>';
        echo '<input type="hidden" name="login">';
        echo '<button class="uibutton">Login</button>';
        echo '</form>';
        echo '<p><a href="register.php?grp='.grouphash($urlgroup).'">Register as new user</a> &nbsp; '.
            '<a href="forgotpw.php?grp='.grouphash($urlgroup).'">I forgot my password</a></p>';
        echo '<p>If you have not used your account for more than one year, you must re-register as a new user to use the system again.</p>';
        echo '</div>';
        print_foot( $style, false );
        die();
    }
    if( isset($_POST['register']) ){
        // we are registering a new user:
        print_head('',$style,$urlgroup);
        echo '<div style="padding: 20px; background-color: #ffffff70;margin: 8px;">';
        $msg = '';
        flock($fp_user, LOCK_EX );
        flock($fp_register, LOCK_EX );
        $block = true;
        if( register_new_user( $_POST['mail'], $_POST['username'], $_POST['password'], $urlgroup, $msg ) ){
            echo '<h2>Account activation:</h2>';
            if( $site['moderation'] )
                echo '<p>An activation request has been issued, you will receive a confirmation email as soon as possible.</p>';
            else
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
    clean_userdb();
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
    // select first active device (if any):
    select_first_active_dev( $_POST['username'] );
    header( "Location: ?grp=".grouphash($urlgroup) );
    die();
}

header( "Location: /?grp=".grouphash($urlgroup) );

?>
