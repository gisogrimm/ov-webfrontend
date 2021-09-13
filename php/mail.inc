<?php
require_once "./../vendor/autoload.php";

use PHPMailer\PHPMailer\PHPMailer;
use PHPMailer\PHPMailer\Exception;

$GLOBALS["smtpConfig"] = [
    "sender" => 'noreply@orlandoviols.com',
    "senderName" => 'OrlandoViols',
];
$GLOBALS["useExternalSMTP"] = false;
$GLOBALS["externalSMTP"] = [
    // Replace smtp_username with your SMTP username.
    "usernameSmtp" => 'smtp_username',
    // Replace smtp_password with your SMTP password.
    "passwordSmtp" => 'smtp_password',
    // Replace smtp host with your SMTP hostname
    "host" => 'smtp_host',
    "port" => 587
];

function sendmail($to, $subject, $message)
{
    if ($GLOBALS["useExternalSMTP"]) {
        $mail = new PHPMailer(true);
        try {
            // Specify the SMTP settings.
            $mail->isSMTP();
            $mail->setFrom($GLOBALS["smtpConfig"]["sender"], $GLOBALS["smtpConfig"]["senderName"]);
            $mail->Username = $GLOBALS["externalSMTP"]["usernameSmtp"];
            $mail->Password = $GLOBALS["externalSMTP"]["passwordSmtp"];
            $mail->Host = $GLOBALS["externalSMTP"]["host"];
            $mail->Port = $GLOBALS["externalSMTP"]["port"];
            $mail->SMTPAuth = true;
            $mail->SMTPSecure = 'tls';

            $mail->addAddress($to);

            $mail->Subject = $subject;
            $mail->Body = $message;
            $mail->Send();
            return true;
        } catch (phpmailerException $e) {
            echo "An error occurred. {$e->errorMessage()}", PHP_EOL; //Catch errors from PHPMailer.
        } catch (Exception $e) {
            echo "Email not sent. {$mail->ErrorInfo}", PHP_EOL; //Catch any other errors
        }
    } else {
        return mail($to, $subject, $message, array('From' => $GLOBALS["smtpConfig"]["senderName"] . ' <' . $GLOBALS["smtpConfig"]["sender"] . '>'));
    }
}