<?php

include '../php/ovbox.inc';

redirect_https();

include '../php/user.inc';
include '../php/rest.inc';
include '../php/session.inc';

print_head( $user, $style, $urlgroup );

echo '<div><span class="ovtitle">Session map</span><div class="help">'.translate('Need help?').' - <a target="blank" href="https://github.com/gisogrimm/ovbox/wiki">Wiki-Pages</a></div></div>';

echo '<div class="devclaim" id="devclaim" style="display:none;"></div>';
echo '<div class="gainhint" id="gainhint" style="display:none;"></div>';

html_show_device( $user, $device, $devprop );

echo '<div class="ovsection" id="sessionmap"></div>';

print_foot($style);

/*
 * Local Variables:
 * c-basic-offset: 2
 * mode: php
 * End:
 */

?>
