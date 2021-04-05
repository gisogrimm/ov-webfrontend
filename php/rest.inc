<?php

/*
 * This file is included from rest.php and user-pages
 */

function get_rooms_user( $user, $userprop, $usergroups, $devroom )
{
  $allrooms = list_db('room');
  ksort($allrooms);
  $drooms = array();
  $mgrooms = array();
  $grooms = array();
  $orooms = array();
  foreach( $allrooms as $room=>$rprop){
    if( ($rprop['age'] < 1000) &&
        ((!$rprop['private']) || ($rprop['owner']==$user)) &&
        ($userprop['seesall'] || ($rprop['owner']==$user) || in_array($rprop['group'],$usergroups)) &&
        (empty($rprop['group']) || (in_array($rprop['group'],$usergroups)))){
      $rprop['id'] = $room;
      $rprop['entered'] = $devroom == $room;
      $roomdev = get_devices_in_room( $room, false, true );
      ksort($roomdev);
      if( $rprop['lock'] && empty($roomdev) ){
        modify_room_prop( $room, 'lock', false );
        $rprop['lock'] = false;
      }
      $roomdevp = array();
      $rprop['roomdev'] = $roomdev;
      if( $rprop['entered'] ){
        $drooms[] = $rprop;
      }else{
        $rprop['lat'] = array();
        if( !empty($rprop['group']) ){
          if( $userprop['maingroup'] == $rprop['group'] )
            $mgrooms[] = $rprop;
          else
            $grooms[] = $rprop;
        }else
          $orooms[] = $rprop;
      }
    }
  }
  $allrooms = array();
  foreach( $drooms as $room )
    $allrooms[] = $room;
  foreach( $mgrooms as $room )
    $allrooms[] = $room;
  foreach( $grooms as $room )
    $allrooms[] = $room;
  foreach( $orooms as $room )
    $allrooms[] = $room;
  return $allrooms;
}

/*
 * Local Variables:
 * c-basic-offset: 2
 * mode: php
 * End:
 */

?>
