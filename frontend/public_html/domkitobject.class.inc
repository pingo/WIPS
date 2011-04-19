<?php

class DOMKitException extends ErrorException { } # Custom exception type for library.

class DOMKitObject
{
	private $document; # The working document object.
	private $xpath;    # An XPath object attached to the above document.
	private $ips;      # An array of nodes used as insertion points.

	function __construct($path)
	{
		$this->document = new DOMDocument('1.0', 'UTF-8');
		$this->document->resolveExternals   = TRUE;
		$this->document->formatOutput		= FALSE;
		$this->document->recover			= FALSE;
		$this->document->preserveWhitespace = FALSE;
		$this->document->validateOnParse	= FALSE; # Turn this off for production.

		$result = $this->document->load($path,
			LIBXML_DTDLOAD | # Load the DTD.
			LIBXML_DTDATTR | # Default DTD attributes.
			LIBXML_COMPACT | # Use compact node optimization.
			LIBXML_NONET   | # Don't get the DTD from the net (too slow).
			LIBXML_NOENT   | # Substitute entities.
			LIBXML_NSCLEAN); # Clean up namespace declarations.

		if ($result === FALSE)
			throw new DOMKitException('could not load xml');

		$this->xpath = new DOMXpath($this->document);
		$nodes = $this->xpath->evaluate("//child::processing-instruction('domkit')");

		foreach ($nodes as $node)
			$this->ips[trim($node->data)] = $node;
	}

	public function load($ipId, $path)
	{
		$ip = $this->ips[$ipId];
		assert('isset($ip)'); # Insertion point must exist.
		unset($this->ips[$ipId]);

		$fragment = $this->document->createDocumentFragment();
		$fragment->appendXML(file_get_contents($path));

		$ip->parentNode->replaceChild($fragment, $ip);

		$this->xpath = new DOMXpath($this->document);
		$nodes = $this->xpath->evaluate("//child::processing-instruction('domkit')", $fragment);

		foreach ($nodes as $node)
			$this->ips[trim($node->data)] = $node;
	}

	private function parse(array $array)
	{
		$tagname = array_shift($array);
		assert('isset($tagname) && is_string($tagname)'); # Tagname must be defined.

		$element = $this->document->createElement($tagname);
		# TODO: Catch invalid character exception - see DOMDocument::createElement manual page.

		foreach ($array as $key => $var)
			if (is_int($key))
				switch ($type = gettype($var))
				{
					case 'boolean':
					case 'integer':
					case 'double':
					case 'string':
					case 'NULL':
						$element->appendChild($this->document->createTextNode($var));
						break;

					case 'array':
						$element->appendChild(self::parse($var));
						break;

					default:
						throw new DOMKitException('child element cannot be of type ' . $type);
				}
			else if (is_string($key) && is_string($var))
				$element->setAttribute($key, $var);
			else
				throw new DOMKitException('attribute name must be string');

		return $element;
	}

	public function insert($ipId, array $array)
	{
		$ip = $this->ips[$ipId];
		assert('isset($ip)'); # Insertion point must exist.
		unset($this->ips[$ipId]);

		foreach ($array as $item)
			$ip->parentNode->insertBefore(self::parse($item), $ip);

		$ip->parentNode->removeChild($ip);
	}

	public function insertText($ipId, $string)
	{
		$ip = $this->ips[$ipId];
		assert('isset($ip)'); # Insertion point must exist.
		unset($this->ips[$ipId]);

		$ip->parentNode->replaceChild($this->document->createTextNode($string), $ip);
	}

	public function saveXML()
	{
		foreach ($this->ips as $ip) # Remove unused insertion points from output.
			$ip->parentNode->removeChild($ip);

		return $this->document->saveXML();
	}
}

