<?php

require_once 'xmlrpc.inc';

$rpc = new XMLRPC('http://localhost:8000/RPC2');

var_export($rpc->{'waitForData'}(time()));
echo PHP_EOL;

