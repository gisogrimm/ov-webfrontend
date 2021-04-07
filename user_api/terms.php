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
include '../php/user.inc';

print_head( );

echo '<div class="room">';
echo '<p><strong><big>Terms of Service for ovbox Services</big></strong></p>';

echo '<p>Thank you for using ovbox services. By accessing the ovbox
interface (https://box.orlandoviols.com/,
https://oldbox.orlandoviols.com/ or http://oldbox.orlandoviols.com/)
either through an internet browser, by using an ovbox device, an
ov-client application, or with any other method, you agree to be bound
by the following terms and conditions.</p>';

echo '<p><strong>Account and Registration</strong></p>';

echo '<p>You may not use the ovbox services and may not accept the terms of
service if you are not of legal age to form a binding contract. If you
are using the ovbox services on behalf of an entity, you represent and
warrant that you have authority to bind that entity to the terms of
service and by accepting the terms of service, you are doing so on
behalf of that entity.</p>';

echo '<p>In order to access certain ovbox services you may be required to
provide certain information (such as identification) as part of the
registration process, or as part of your continued use of the ovbox
services.</p>';

echo '<p><strong>Privacy</strong></p>';

echo '<p>We do not store any data except what is absolutely neccesary for
providing the ovbox services. This data may include your user name as
defined during the registration process, the date and time of the last
access to the service, and the MAC address of your ovbox devices or
other devices running the ov-client application.</p>';

echo '<p><strong>Liability</strong></p>';

echo '<p>Because the services are provided free of charge, there is no
warranty for the services, to the extent permitted by applicable
law. The services are provided "as is" and without warranty of any kind,
either expressed or implied, including but not limited to the
implied warranties of merchantability and fitness for a particular
purpose. The entire risk as to the quality and performance of the
services is with you.</p>';

echo '<p>In no event will any provider of the ovbox services be liable to
you for damages, including any general, special, incidental or
consequential damages arising out of the use of or inability to use
the ovbox servives (including, but not limited to, loss of data,
inaccurate data, or loss sustained by you or third parties), even if
such provider of services has been advised of the possibility of such
damages.</p>';

echo '<p><strong>Termination</strong></p>';

echo '<p>The Provider may discontinue the ovbox services or parts thereof at
any time.</p>';
echo '<p>As a user of the ovbox Services, you may terminate your account at
any time via the website or by e-mail. By terminating an account, all
associated data is irretrievably deleted.</p>';
echo '<p>Inactive accounts are automatically deleted. Accounts without a
linked device are considered inactive if the user has not logged on
within the last 30 days. Accounts with a linked device are considered
inactive if the user has not accessed the ovbox services within the
last 180 days.</p>';

echo '</div>';

print_foot('',false);

?>
