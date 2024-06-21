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

print_head( $user, $style, $urlgroup, false );

echo '</div>';
echo '<div class="sessionvid" id="sessionvid">';

function rotate( $rcos, $rsin, &$x, &$y,$posx,$posy )
{
    $tmpx = $rcos*$x + $rsin*$y;
    $tmpy = -$rsin*$x + $rcos*$y;
    $x = $tmpx + $posx;
    $y = $tmpy + $posy;
}

function get_devh( $device, $room )
{
  $dh = hash('md5',$device . $room);
  return $dh;
}

if( !empty($devprop['room'])){

  $rdevs = get_devices_in_room( $devprop['room'], true, true );
  $x0 = $devprop['position']['x'];
  $y0 = $devprop['position']['y'];
  $rcos = cos(deg2rad($devprop['orientation']['z']));
  $rsin = sin(deg2rad($devprop['orientation']['z']));
  $idx = 0;
  $vids = array();
  foreach( $rdevs as $did=>$dprop ){
    $idx = $idx+1;
    $d = $dprop['id'];
    $dh = get_devh( $d, $devprop['room']);
    $x = 50*($dprop['position']['x']-$x0)/2.4;
    $y = 50*($dprop['position']['y']-$y0)/2.4;
    rotate($rcos,$rsin,$x,$y,0,0);
    $x = round($x);
    $y = round($y);
    $vids[-$x][-$y] = $dprop;
    //error_log('x='.$x.' y='.$y.' '.$dprop['label']);
    $y = -0.78*$y+50-11;
    $x = 50-$x;
  }
  ksort($vids);
  $doc = new DOMDocument('1.0');
  $root = $doc->appendChild($doc->createElement('div'));
  $r = 0;
  foreach($vids as $row){
    $r = $r + 1;
    ksort($row);
    $divr = $root->appendChild($doc->createElement('div'));
    $divr->setAttribute('class','vidrow');
    //error_log('row');
    $k = 0;
    foreach($row as $vid){
      $k = $k+1;
      $d = $vid['id'];
      $dh = get_devh( $d, $devprop['room']);
      if( ($k > 1) && ($r > 1) ){
        $divv = $divr->appendChild($doc->createElement('div'));
        $divv->setAttribute('class','vidspacer');
      }
      $divv = $divr->appendChild($doc->createElement('div'));
      $divv->setAttribute('class','vidcont');
      $divv->setAttribute('id','vcon.'.$d);
      $ifr = $divv->appendChild($doc->createElement('iframe'));
      $url = 'https://vdo.ninja/?view='.$dh;
      $zidx = 'z-index:'.$idx.'; ';
      if( $d == $device ){
        $ifr->setAttribute('allow','camera;display-capture');
        $url = 'https://vdo.ninja/?push='.$dh.'&ad=0';
      }
      //$url = 'https://orlandoviols.com/';
      $ifr->setAttribute('src',$url);
      $ifr->setAttribute('class','filldiv');
      $ifr->setAttribute('id','vid.'.$d);
      $dname = $divv->appendChild($doc->createElement('input'));
      $dname->setAttribute('class','viddevlabel');
      $dname->setAttribute('value',$vid['label']);
      $dname->setAttribute('type','button');
      $dname->setAttribute('onclick','vid_toggle_max("'.$d.'");');
      //error_log($vid['label']);
    }
  }
  echo $doc->saveHTML() . "\n";
}

print_foot($style,false);

/*
 * Local Variables:
 * c-basic-offset: 2
 * mode: php
 * End:
 */

?>
