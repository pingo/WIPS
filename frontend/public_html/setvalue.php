<?php

require_once 'config.inc';
require_once 'xmlrpc.inc';

spl_autoload_extensions('.class.inc');
spl_autoload_register();

$rpc = new XMLRPC($cfg_rpc);

$v = (int)abs($_POST['value']);

switch ($_POST['action'])
{
	case 'delta':
		$result = $rpc->setDelta($_POST['mote'], $v);
		break;

	case 'timeout':
		$result = $rpc->setSensorTimeout($_POST['mote'], $v);
		break;

	case 'sinterval':
		$result = $rpc->setSensorInterval($_POST['mote'], $v);
		break;

	case 'binterval':
		$result = $rpc->setBeaconInterval($_POST['mote'], $v);
		break;

	default:
		header('HTTP/1.0 500 Internal Server Error', 500);
		break;
}

if (!$result)
	header('HTTP/1.0 500 Internal Server Error', 500);

