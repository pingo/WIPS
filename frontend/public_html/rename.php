<?php

$db = new PDO('sqlite:../../backend/backend.db');

list($major, $minor) = explode('.', $_POST['mote'], 2);

$stmt = $db->prepare('UPDATE `mote` SET `name` = ? WHERE `major` = ? AND `minor` = ?');

if ($stmt !== FALSE)
	$stmt->execute(array($_POST['name'], $major, $minor)) or die(var_export($stmt->errorInfo(), TRUE));
else
	die(var_export($db->errorInfo(), TRUE));

header('Location: /?mote=' . urlencode($_POST['mote']));

