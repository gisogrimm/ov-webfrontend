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

$urlgroup = '';
if( isset($_GET['grp'] ) )
    $urlgroup = get_group_by_hash($_GET['grp']);
    $style = '';
if( !empty($urlgroup) ){
    $gprop = get_properties( $urlgroup, 'group' );
    $style = $gprop['style'];
}

$site = get_properties('site','config');

$title = 'Register as new user:';
if($site['moderation'])
    $title = 'Request registration from moderator:';

print_head('',$style);
// display registration box: username, username, password
echo '<div style="padding: 20px; background-color: #ffffff70;margin: 8px;">';
echo '<h2>'.$title.'</h2>'."\n";
echo '<form name="register" class="login" action="/login.php?grp='.grouphash($urlgroup).'" method="POST">'."\n";
echo '<label>e-mail address:</label><br>';
echo '<input type="text" name="mail" required><br>';
echo '<label>User name:</label><br>';
echo '<input type="text" name="username" pattern="[a-zA-Z0-9-_]*" title="only letters and numbers" required><br>';
echo '<label>Password:</label><br>';
echo '<input type="password" name="password" required><br>';
echo '<span style="font-size: 70%;"><input type="checkbox" name="agreepriv" required><label for="agreepriv">I have read and accept the <a target="blank" href="privacy.php">privacy policy</a> / <a target="blank" href="datenschutz.php">Datenschutzerkärung</a>.</label><br>';
echo '<input type="checkbox" name="agreeterms" required><label for="agreeterms">I have read and accept the <a target="blank" href="terms.php">terms of service</a>.</label></span><br>';
echo '<input type="hidden" name="register">';
echo '<input type="submit" value="Register">';
echo '</form>';
echo '<p><a href="/?grp='.grouphash($urlgroup).'">Back to login page</a></p>';
echo '</div>';
    
print_foot( $style, false );

?>
