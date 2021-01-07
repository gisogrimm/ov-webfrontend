<?php

include '../php/ovbox.inc';

$urlgroup = '';
if( isset($_GET['grp'] ) )
    $urlgroup = get_group_by_hash($_GET['grp']);
    $style = '';
if( !empty($urlgroup) ){
    $gprop = get_properties( $urlgroup, 'group' );
    $style = $gprop['style'];
}

print_head('',$style);
// display registration box: username, username, password
echo '<div style="padding: 20px; background-color: #ffffff70;margin: 8px;">';
echo '<h2>Reset password:</h2>';
echo '<p>If you registered with a valid email address, you may reset your password. A confirmation mail will be sent to the address linked to your account.</p>';
echo '<form class="login" action="/?grp='.grouphash($urlgroup).'" method="POST">';
echo '<label>User name:</label><br>';
echo '<input type="text" name="pwrusername" pattern="[a-zA-Z0-9-_]*" title="only letters and numbers"><br>';
echo '<input type="hidden" name="forgotpw">';
echo '<button>Request password reset</button>';
echo '</form>';
echo '<p><a href="/?grp='.grouphash($urlgroup).'">Back to login page</a></p>';
echo '</div>';

print_foot( $style, false );


?>