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

echo '<div class="sessionvid" id="sessionvid">';

function rotate( $rcos, $rsin, &$x, &$y,$posx,$posy )
{
    $tmpx = $rcos*$x + $rsin*$y;
    $tmpy = -$rsin*$x + $rcos*$y;
    $x = $tmpx + $posx;
    $y = $tmpy + $posy;
}

if( !empty($devprop['room'])){

  $rdevs = get_devices_in_room( $devprop['room'], true, true );
  $x0 = $devprop['position']['x'];
  $y0 = $devprop['position']['y'];
  $rcos = cos(deg2rad($devprop['orientation']['z']));
  $rsin = sin(deg2rad($devprop['orientation']['z']));
  $idx = 0;
  foreach( $rdevs as $did=>$dprop ){
    $idx = $idx+1;
    $d = $dprop['id'];
    $dh = hash('md5',$d);
    $x = 50*($dprop['position']['x']-$x0)/2.4;
    $y = 50*($dprop['position']['y']-$y0)/2.4;
    rotate($rcos,$rsin,$x,$y,0,0);
    $y = -0.78*$y+50-11;
    $x = 50-$x;
    $url = 'https://vdo.ninja/?view='.$dh;
    $allow = '';
    $zidx = 'z-index:'.$idx.'; ';
    if( $d == $device ){
      $url = 'https://vdo.ninja/?push='.$dh.'&ad=0';
      $allow = ' allow="camera" ';
      $zidx = 'z-index:0; ';
    }
    echo '<div id="vcon.'.$d.'" class="vidcont" style="'.$zidx.'left:'.$y.'%; top:'.$x.'%;">';
    echo '<iframe id="vid'.$d.'" style="position: absolute; height: 100%; border: none" width="100%" height="100%" '.$allow.' src="'.$url.'"></iframe>';
    echo '<input class="viddevlabel" value="'.$dprop['label'].'" type="button" onclick="vid_toggle_max(\''.$d.'\');"></input>';
    echo '</div>';
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
