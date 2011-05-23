<?php

class XMLRPCException extends Exception
{
	public $faultCode;
	public $faultString;

	function __construct($faultCode, $faultString)
	{
		$this->faultCode = $faultCode;
		$this->faultString = $faultString;
	}

	function __toString()
	{
		return __CLASS__ . ': [' . $this->faultCode . ']: ' . $this->faultString . PHP_EOL;
	}
}

class XMLRPC_Raw
{
	private $data;

	function __construct($data)
	{
		$this->data = $data;
	}

	function __toString()
	{
		return $this->data;
	}
}

class XMLRPC_Time
{
	private $data;

	function __construct($data)
	{
		$this->data = $data;
	}
}

class XMLRPC
{
	private $curl;

	function __construct($uri)
	{
		$this->curl = curl_init($uri);

		curl_setopt_array($this->curl, array(
			CURLOPT_PROTOCOLS      => CURLPROTO_HTTP | CURLPROTO_HTTPS,
			CURLOPT_HEADER         => FALSE,
			CURLOPT_RETURNTRANSFER => TRUE,
			CURLOPT_BINARYTRANSFER => TRUE,
			CURLOPT_POST           => TRUE,
			CURLOPT_USERAGENT      => 'Web-rTorrent/1.0',
			CURLOPT_HTTPHEADER     => array('Accept: text/xml', 'Content-Type: text/xml; charset=UTF-8', 'Expect:')
		));
	}

	function __deconstruct()
	{
		curl_close($this->curl);
	}

	function __call($method, $args)
	{
		$opts = array('encoding'  => 'utf-8',
		              'escaping'  => 'markup',
		              'verbosity' => 'pretty',
		              'version'   => 'xmlrpc');

		foreach ($args as &$arg)
		{
			if (is_a($arg, 'XMLRPC_Raw'))
			{
				$arg = (string)$arg;
				xmlrpc_set_type($arg, 'base64');
			}
			else if (is_a($arg, 'XMLRPC_Time'))
			{
				$arg = (string)$arg;
				xmlrpc_set_type($arg, 'datetime');
				throw new XMLRPCException(5, 'could not parse datetime');
			}
			else if (is_object($arg))
				throw new XMLRPCException(1, 'invalid argument type');
		}

		$request = xmlrpc_encode_request($method, $args, $opts);

		curl_setopt($this->curl, CURLOPT_POSTFIELDS, $request);

		if (($response = curl_exec($this->curl)) === FALSE)
			throw new XMLRPCException(2, 'connection error');

		/* Workaround for unsupported i8 type. */
		$response = str_replace('i8>', 'i4>', $response);

		if (($value = xmlrpc_decode($response)) === NULL)
			throw new XMLRPCException(3, 'could not decode response');

		if (is_array($value) && xmlrpc_is_fault($value))
			throw new XMLRPCException($value['faultCode'], $value['faultString']);

		return $value;
	}
}

