<?php

if( !(isset($_SERVER['HTTPS']) && $_SERVER['HTTPS'] === 'on') ){
    if( substr_compare( $_SERVER['HTTP_HOST'], 'localhost', 0, 9 )!= 0){
        $actual_link = "https://$_SERVER[HTTP_HOST]$_SERVER[REQUEST_URI]";
        header( "Location: ".$actual_link );
        die();
    }
}

include '../php/ovbox.inc';

print_head( );

echo '<div class="room">';
echo '<p><strong><big>Privacy policy</big></strong></p>';

echo '<p><strong>General note and mandatory information</strong></p>';
echo '<p><strong>Designation of the responsible body</strong></p>';

echo '<p>The person responsible for data processing on this website is</p>';
echo '<p><span id="s3-t-firma">Ensemble ORLANDOviols</span><br><span id="s3-t-ansprechpartner">Giso Grimm</span><br><span id="s3-t-strasse">Elsässer Straße 8</span><br><span id="s3-t-plz">26121</span> <span id="s3-t-ort">Oldenburg</span><br>Germany</p><p></p>';
echo '<p>The responsible body decides alone or jointly with others on the purposes and means of processing personal data (e.g. names, contact details, etc.).</p>';

echo '<p><strong>Revocation of your consent to data processing</strong></p>';

echo '<p>Only with your express consent are some data processing operations possible. A revocation of your already given consent is possible at any time. An informal notification by e-mail is sufficient for the revocation. The lawfulness of the data processing carried out up to the revocation remains unaffected by the revocation.</p>';

echo '<p><strong>Right to appeal to the competent supervisory authority</strong></p>';

echo '<p>As the person concerned, you have the right to complain to the responsible supervisory authority in the event of a violation of data protection laws. The competent supervisory authority with regard to data protection issues is the data protection commissioner of the federal state in which our company is located. The following link provides a list of the data protection officers and their contact details: <a href="https://www.bfdi.bund.de/DE/Infothek/Anschriften_Links/anschriften_links-node.html" target="_blank">https://www.bfdi.bund.de/DE/Infothek/Anschriften_Links/anschriften_links-node.html</a>.</p>';
echo '<p><strong>Right to data transferability</strong></p>';
echo '<p>You have the right to have data, which we process automatically on the basis of your consent or in fulfilment of a contract, handed over to you or to third parties. The data will be provided in a machine-readable format. If you request the direct transfer of the data to another responsible party, this will only be done as far as it is technically feasible.</p>';

echo '<p><strong>Right to information, correction, blocking, deletion</strong></p>';
echo '<p>Within the framework of the applicable legal provisions, you have the right to obtain information free of charge at any time about your stored personal data, the origin of the data, its recipients and the purpose of the data processing and, if applicable, a right to correct, block or delete this data. In this regard and also for further questions on the subject of personal data, you can contact us at any time using the contact options listed in the imprint.</p>';

echo '<p><strong>SSL or TLS encryption</strong></p>';
echo '<p>For security reasons and to protect the transmission of confidential content that you send to us as the site operator, our website uses SSL or TLS encryption. This means that data that you transmit via this website cannot be read by third parties. You can recognize an encrypted connection by the "https://" address line of your browser and the lock symbol in the browser line.</p>';

echo '<p><strong>Server log files</strong></p>';
echo '<p>In server log files, the provider of the website automatically collects and stores information that your browser automatically sends to us. These are:</p>';
echo '<ul>';
echo '  <li>Visited page on our domain</li>';
echo '  <li>Date and time of the server request</li>';
echo '  <li>Browser type and version</li>';
echo '  <li>Operating system used</li>';
echo '  <li>Referrer URL</li>';
echo '  <li>Hostname of the accessing computer</li>';
echo '  <li>IP address</li>';
echo '</ul>';
echo '<p>This data is not merged with other data sources. The basis for data processing is Art. 6 para. 1 lit. b GDPR, which permits the processing of data for the fulfilment of a contract or pre-contractual measures.</p>';
echo '';
echo '<p><strong>Registration on this website</strong></p>';
echo '<p>You can register on our website to use certain functions. The transmitted data is used exclusively for the purpose of using the respective offer or service. Mandatory data requested during registration must be provided in full. Otherwise we will refuse the registration.</p>';
echo '<p>In case of important changes, for example for technical reasons, we will inform you by e-mail. The e-mail will be sent to the address given during registration.</p>';
echo '<p>The data entered during registration will be processed on the basis of your consent (Art. 6 para. 1 lit. a GDPR). A revocation of your already given consent is possible at any time. An informal notification by e-mail is sufficient for the revocation. The legality of the data processing already carried out remains unaffected by the revocation.</p>';
echo '<p>We store the data collected during registration for the period of time you are registered on our website. Your data will be deleted if you cancel your registration. Legal retention periods remain unaffected.</p>';
echo '';
echo '<p><strong>Contact form</strong></p>';
echo '<p>Data transmitted via the contact form will be stored including your contact data in order to be able to process your inquiry or to be available for follow-up questions. These data will not be passed on without your consent.</p>';
echo '<p>The processing of the data entered in the contact form is based exclusively on your consent (Art. 6 para. 1 lit. a GDPR). A revocation of your already given consent is possible at any time. An informal notification by e-mail is sufficient for the revocation. The legality of the data processing operations carried out until the revocation remains unaffected by the revocation.</p>';
echo '<p>Data transmitted via the contact form will remain with us until you request us to delete it, revoke your consent to its storage or until there is no longer any need for data storage. Mandatory legal provisions - in particular retention periods - remain unaffected.</p>';

echo '<p><strong>PayPal</strong></p>';
echo '<p>Our website allows payment via PayPal. Provider of the payment service is PayPal (Europe) S.à.r.l. et Cie, S.C.A., 22-24 Boulevard Royal, L-2449 Luxembourg.</p>';
echo '<p>If you pay with PayPal, the payment data you entered will be transmitted to PayPal.</p>';
echo '<p>The transmission of your data to PayPal is based on art. 6 para. 1 lit. a GDPR (consent) and art. 6 para. 1 lit. b GDPR (processing for the performance of a contract). A revocation of your already given consent is possible at any time. Data processing procedures in the past remain effective in the event of a revocation.';
echo '<p><small>Source: Datenschutz-Konfigurator von <a href="http://www.mein-datenschutzbeauftragter.de" target="_blank">mein-datenschutzbeauftragter.de</a></small></p>';
echo '</div>';

print_foot('',false);

?>
