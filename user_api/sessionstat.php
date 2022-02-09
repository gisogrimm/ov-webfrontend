<?php

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

include '../php/ovbox.inc';
include '../php/user.inc';
include '../php/rest.inc';
include '../php/session.inc';

print_head( $user, $style, $urlgroup );

echo '<div><span class="ovtitle">Session statistics</span><div class="help">Need help? - <a target="blank" href="https://github.com/gisogrimm/ovbox/wiki">Wiki-Pages</a> / <a target="blank" href="https://forum.digital-stage.org/">DS-Forum</a></div></div>';

echo '<div class="devclaim" id="devclaim" style="display:none;"></div>';

html_show_device( $user, $device, $devprop );

echo '<div class="warning">This page is still under development. The displayed data may be invalid.</div>';

echo '<div id="sessionstat"></div>';

print_foot($style);

/*
 * Local Variables:
 * c-basic-offset: 2
 * mode: php
 * End:
 */

?>
