<?php
include '../php/ovbox.inc';
include '../php/rest.inc';

session_start();
if( !isset($_SESSION['user']) )
    die();
$user = $_SESSION['user'];
flock($fp_user, LOCK_EX );
if( !in_array($user,list_users()) ){
    flock($fp_user, LOCK_UN );
    die();
}
$uprop = get_properties( $user, 'user' );
if( isset($_GET['getuser']) ){
    echo(json_encode($uprop));
    flock($fp_user, LOCK_UN );
    die();
}
flock($fp_dev, LOCK_EX );
$device = get_device( $user );
flock($fp_user, LOCK_UN );
$dprop = get_properties( $device, 'device' );
if( $dprop['owner'] != $user ){
    $device = '';
    $dprop = get_properties( $device, 'device' );
}
if( empty($dprop['owner']) )
    $dprop['owner'] = $user;

function svgtext( $x, $y, $s, $txt, $attr = '' )
{
    echo '<text x="'.$x.'" y="'.$y.'" font-family="Sans,Helvetica,Arial" font-size="'.$s.'" '.$attr.'><tspan>'.$txt.'</tspan></text>';
}

function rotate( $rcos, $rsin, &$x, &$y,$posx,$posy )
{
    $tmpx = $rcos*$x + $rsin*$y;
    $tmpy = -$rsin*$x + $rcos*$y;
    $x = $tmpx + $posx;
    $y = $tmpy + $posy;
}

function svgdevice( $dev, $prop, $thisdev )
{
    if( $prop['isactive'] ){
        $colbg = '#ecc348';
        if( !$prop['issender'] )
            $colbg = '#4096d6';
        $r = 15;
    }else{
        $colbg = '#d6d6d6';
        if( !$prop['issender'] )
            $colbg = '#abc4d6';
        $r = 10;
    }
    $pos = $prop['position'];
    $posx = 100*$pos['x'];
    $posy = -100*$pos['y'];
    $rcos = cos(deg2rad($prop['orientation']['z']));
    $rsin = sin(deg2rad($prop['orientation']['z']));
    $me = $prop['id'] == $thisdev;
    foreach( $prop['inputchannels'] as $k=>$ch ){
        $cx = 100*$ch['position']['x'];
        $cy = 100*$ch['position']['y'];
        rotate($rcos,$rsin,$cx,$cy,$posx,$posy);
        $path = 'M'.$posx.' '.$posy.' L'.$cx.' '.$cy;
        echo '<path d="'.$path.'" stroke="#000"/>';
    }
    if( $me ){
        $r = $r*2;
        $nx1 = $r*0.9;
        $nx2 = $r*1.45;
        $nx3 = $r*0.9;
        $ny1 = -$r*0.3;
        $ny2 = 0;
        $ny3 = $r*0.3;
        rotate($rcos,$rsin,$nx1,$ny1,$posx,$posy);
        rotate($rcos,$rsin,$nx2,$ny2,$posx,$posy);
        rotate($rcos,$rsin,$nx3,$ny3,$posx,$posy);
        echo '<polyline points="'.$nx1.','.$ny1.' '.$nx2.','.$ny2.' '.$nx3.','.$ny3.'" fill="'.$colbg.'" stroke="#000" stroke-width="3"/>';
    }
    echo '<circle cx="'.$posx.'" cy="'.$posy.'" r="'.$r.'" fill="'.$colbg.'" stroke="#000" stroke-width="3"/>';
    if( $me ){
        $nx1 = -$r*0.4;
        $nx2 = 0;
        $nx3 = $r*0.4;
        $ny1 = $r*1.2;
        $ny2 = $r*0.8;
        $ny3 = $r*1.2;
        rotate($rcos,$rsin,$nx1,$ny1,$posx,$posy);
        rotate($rcos,$rsin,$nx2,$ny2,$posx,$posy);
        rotate($rcos,$rsin,$nx3,$ny3,$posx,$posy);
        echo '<polyline points="'.$nx1.','.$ny1.' '.$nx2.','.$ny2.' '.$nx3.','.$ny3.'" fill="none" stroke="#000" stroke-width="3"/>';
        $nx1 = -$r*0.4;
        $nx2 = 0;
        $nx3 = $r*0.4;
        $ny1 = -$r*1.2;
        $ny2 = -$r*0.8;
        $ny3 = -$r*1.2;
        rotate($rcos,$rsin,$nx1,$ny1,$posx,$posy);
        rotate($rcos,$rsin,$nx2,$ny2,$posx,$posy);
        rotate($rcos,$rsin,$nx3,$ny3,$posx,$posy);
        echo '<polyline points="'.$nx1.','.$ny1.' '.$nx2.','.$ny2.' '.$nx3.','.$ny3.'" fill="none" stroke="#000" stroke-width="3"/>';
    }
    foreach( $prop['inputchannels'] as $k=>$ch ){
        $cx = 100*$ch['position']['x'];
        $cy = 100*$ch['position']['y'];
        rotate($rcos,$rsin,$cx,$cy,$posx,$posy);
        echo '<circle cx="'.$cx.'" cy="'.$cy.'" r="6" fill="'.$colbg.'" stroke="#777"/>';
    }
    svgtext($posx+$r,$posy-$r-1,16,$prop['label']);
}

header('Content-Type: image/svg+xml');
echo '<?xml version="1.0" standalone="no"?><svg width="100%" height="100%" version="1.1" viewBox="-200 -200 400 400" xmlns="http://www.w3.org/2000/svg">';
if( !empty($dprop['room']) ){
    $rprop = get_properties($dprop['room'],'room');
    $roomdev = get_devices_in_room( $dprop['room'], false, true );
    svgtext( -198, -180, 20, $rprop['label']);
    //echo '<rect fill="none" stroke="#000" x="-299" y="-299" width="598" height="598" stroke-width="1"/>';
    foreach( $roomdev as $dev=>$prop )
        svgdevice($dev, $prop, $device );
}
echo '</svg>';
?>
