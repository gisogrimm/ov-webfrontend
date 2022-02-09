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
include '../php/rest.inc';
include '../php/user.inc';

print_head( );

echo '<div class="room">';
echo '<p><strong><big>Datenschutzerklärung</big></strong></p>';
echo '<p><strong>Allgemeiner Hinweis und Pflichtinformationen</strong></p>';
echo '<p><strong>Benennung der verantwortlichen Stelle</strong></p>';

echo '<p>Die verantwortliche Stelle für die Datenverarbeitung auf dieser Website ist:</p>';

echo '<p><span id="s3-t-firma">Ensemble ORLANDOviols</span><br><span id="s3-t-ansprechpartner">Giso Grimm</span><br><span id="s3-t-strasse">Elsässer Straße 8</span><br><span id="s3-t-plz">D-26121</span> <span id="s3-t-ort">Oldenburg</span></p><p></p>';

echo '<p>Die verantwortliche Stelle entscheidet allein oder gemeinsam mit anderen über die Zwecke und Mittel der Verarbeitung von personenbezogenen Daten (z.B. Namen, Kontaktdaten o. Ä.).</p>';

echo '<p><strong>Widerruf Ihrer Einwilligung zur Datenverarbeitung</strong></p>';
echo '<p>Nur mit Ihrer ausdrücklichen Einwilligung sind einige Vorgänge der Datenverarbeitung möglich. Ein Widerruf Ihrer bereits erteilten Einwilligung ist jederzeit möglich. Für den Widerruf genügt eine formlose Mitteilung per E-Mail. Die Rechtmäßigkeit der bis zum Widerruf erfolgten Datenverarbeitung bleibt vom Widerruf unberührt.</p>';
echo '<p><strong>Recht auf Beschwerde bei der zuständigen Aufsichtsbehörde</strong></p>';
echo '<p>Als Betroffener steht Ihnen im Falle eines datenschutzrechtlichen Verstoßes ein Beschwerderecht bei der zuständigen Aufsichtsbehörde zu. Zuständige Aufsichtsbehörde bezüglich datenschutzrechtlicher Fragen ist der Landesdatenschutzbeauftragte des Bundeslandes, in dem sich der Sitz unseres Unternehmens befindet. Der folgende Link stellt eine Liste der Datenschutzbeauftragten sowie deren Kontaktdaten bereit: <a href="https://www.bfdi.bund.de/DE/Infothek/Anschriften_Links/anschriften_links-node.html" target="_blank">https://www.bfdi.bund.de/DE/Infothek/Anschriften_Links/anschriften_links-node.html</a>.</p>';
echo '<p><strong>Recht auf Datenübertragbarkeit</strong></p>';
echo '<p>Ihnen steht das Recht zu, Daten, die wir auf Grundlage Ihrer Einwilligung oder in Erfüllung eines Vertrags automatisiert verarbeiten, an sich oder an Dritte aushändigen zu lassen. Die Bereitstellung erfolgt in einem maschinenlesbaren Format. Sofern Sie die direkte Übertragung der Daten an einen anderen Verantwortlichen verlangen, erfolgt dies nur, soweit es technisch machbar ist.</p>';
echo '<p><strong>Recht auf Auskunft, Berichtigung, Sperrung, Löschung</strong></p>';
echo '<p>Sie haben jederzeit im Rahmen der geltenden gesetzlichen Bestimmungen das Recht auf unentgeltliche Auskunft über Ihre gespeicherten personenbezogenen Daten, Herkunft der Daten, deren Empfänger und den Zweck der Datenverarbeitung und ggf. ein Recht auf Berichtigung, Sperrung oder Löschung dieser Daten. Diesbezüglich und auch zu weiteren Fragen zum Thema personenbezogene Daten können Sie sich jederzeit über die im Impressum aufgeführten Kontaktmöglichkeiten an uns wenden.</p>';
echo '<p><strong>SSL- bzw. TLS-Verschlüsselung</strong></p>';
echo '<p>Aus Sicherheitsgründen und zum Schutz der Übertragung vertraulicher Inhalte, die Sie an uns als Seitenbetreiber senden, nutzt unsere Website eine SSL-bzw. TLS-Verschlüsselung. Damit sind Daten, die Sie über diese Website übermitteln, für Dritte nicht mitlesbar. Sie erkennen eine verschlüsselte Verbindung an der „https://“ Adresszeile Ihres Browsers und am Schloss-Symbol in der Browserzeile.</p>';
echo '<p><strong>Server-Log-Dateien</strong></p>';
echo '<p>In Server-Log-Dateien erhebt und speichert der Provider der Website automatisch Informationen, die Ihr Browser automatisch an uns übermittelt. Dies sind:</p>';
echo '<ul>';
echo '  <li>Besuchte Seite auf unserer Domain</li>';
echo '  <li>Datum und Uhrzeit der Serveranfrage</li>';
echo '  <li>Browsertyp und Browserversion</li>';
echo '  <li>Verwendetes Betriebssystem</li>';
echo '  <li>Referrer URL</li>';
echo '  <li>Hostname des zugreifenden Rechners</li>';
echo '  <li>IP-Adresse</li>';
echo '</ul>';
echo '<p>Es findet keine Zusammenführung dieser Daten mit anderen Datenquellen statt. Grundlage der Datenverarbeitung bildet Art. 6 Abs. 1 lit. b DSGVO, der die Verarbeitung von Daten zur Erfüllung eines Vertrags oder vorvertraglicher Maßnahmen gestattet.</p>';
echo '';
echo '<p><strong>Registrierung auf dieser Website</strong></p>';
echo '<p>Zur Nutzung bestimmter Funktionen können Sie sich auf unserer Website registrieren. Die übermittelten Daten dienen ausschließlich zum Zwecke der Nutzung des jeweiligen Angebotes oder Dienstes. Bei der Registrierung abgefragte Pflichtangaben sind vollständig anzugeben. Andernfalls werden wir die Registrierung ablehnen.</p>';
echo '<p>Im Falle wichtiger Änderungen, etwa aus technischen Gründen, informieren wir Sie per E-Mail. Die E-Mail wird an die Adresse versendet, die bei der Registrierung angegeben wurde.</p>';
echo '<p>Die Verarbeitung der bei der Registrierung eingegebenen Daten erfolgt auf Grundlage Ihrer Einwilligung (Art. 6 Abs. 1 lit. a DSGVO). Ein Widerruf Ihrer bereits erteilten Einwilligung ist jederzeit möglich. Für den Widerruf genügt eine formlose Mitteilung per E-Mail. Die Rechtmäßigkeit der bereits erfolgten Datenverarbeitung bleibt vom Widerruf unberührt.</p>';
echo '<p>Wir speichern die bei der Registrierung erfassten Daten während des Zeitraums, den Sie auf unserer Website registriert sind. Ihren Daten werden gelöscht, sollten Sie Ihre Registrierung aufheben. Gesetzliche Aufbewahrungsfristen bleiben unberührt.</p>';
echo '            ';
echo '<p><strong>Kontaktformular</strong></p>';
echo '<p>Per Kontaktformular übermittelte Daten werden einschließlich Ihrer Kontaktdaten gespeichert, um Ihre Anfrage bearbeiten zu können oder um für Anschlussfragen bereitzustehen. Eine Weitergabe dieser Daten findet ohne Ihre Einwilligung nicht statt.</p>';
echo '<p>Die Verarbeitung der in das Kontaktformular eingegebenen Daten erfolgt ausschließlich auf Grundlage Ihrer Einwilligung (Art. 6 Abs. 1 lit. a DSGVO). Ein Widerruf Ihrer bereits erteilten Einwilligung ist jederzeit möglich. Für den Widerruf genügt eine formlose Mitteilung per E-Mail. Die Rechtmäßigkeit der bis zum Widerruf erfolgten Datenverarbeitungsvorgänge bleibt vom Widerruf unberührt.</p>';
echo '<p>Über das Kontaktformular übermittelte Daten verbleiben bei uns, bis Sie uns zur Löschung auffordern, Ihre Einwilligung zur Speicherung widerrufen oder keine Notwendigkeit der Datenspeicherung mehr besteht. Zwingende gesetzliche Bestimmungen - insbesondere Aufbewahrungsfristen - bleiben unberührt.</p>';
echo '<p><strong>PayPal</strong></p>';
echo '<p>Unsere Website ermöglicht die Bezahlung via PayPal. Anbieter des Bezahldienstes ist die PayPal (Europe) S.à.r.l. et Cie, S.C.A., 22-24 Boulevard Royal, L-2449 Luxembourg.</p>';
echo '<p>Wenn Sie mit PayPal bezahlen, erfolgt eine Übermittlung der von Ihnen eingegebenen Zahlungsdaten an PayPal.</p>';
echo '<p>Die Übermittlung Ihrer Daten an PayPal erfolgt auf Grundlage von Art. 6 Abs. 1 lit. a DSGVO (Einwilligung) und Art. 6 Abs. 1 lit. b DSGVO (Verarbeitung zur Erfüllung eines Vertrags). Ein Widerruf Ihrer bereits erteilten Einwilligung ist jederzeit möglich.';
echo '  In der Vergangenheit liegende Datenverarbeitungsvorgänge bleiben bei einem Widerruf wirksam.</p>';
echo '<p><small>Quelle: Datenschutz-Konfigurator von <a href="http://www.mein-datenschutzbeauftragter.de" target="_blank">mein-datenschutzbeauftragter.de</a></small></p>';
echo '</div>';

print_foot('',false);

?>
