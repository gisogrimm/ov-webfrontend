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

include '../php/user.inc';
include '../php/rest.inc';
include '../php/session.inc';

print_head( $user, $style, $urlgroup );

echo '<div><span class="ovtitle">Session videos</span><div class="help">Need help? - <a target="blank" href="https://github.com/gisogrimm/ovbox/wiki">Wiki-Pages</a> / <a target="blank" href="https://forum.digital-stage.org/">DS-Forum</a></div></div>';

echo '<div class="devclaim" id="devclaim" style="display:none;"></div>';

//html_show_device( $user, $device, $devprop );

echo '<div class="sessionvid">';

if( !empty($devprop['room'])){

  $rdevs = get_devices_in_room( $devprop['room'], true, false );
  foreach( $rdevs as $d ){
    $dh = hash('md5',$d);
    if( $d == $device ){
      echo '<iframe id="vid'.$d.'" allow="camera" src="https://vdo.ninja/?push='.$dh.'&ad=0"></iframe>';
    }else{
      echo '<iframe id="vid'.$d.'" src="https://vdo.ninja/?view='.$dh.'"></iframe>';
    }
  }

}

echo '</div>';

print_foot($style);

/*
 * Local Variables:
 * c-basic-offset: 2
 * mode: php
 * End:
 */

?>
