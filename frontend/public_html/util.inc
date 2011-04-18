<?php

function time_ago($datefrom, $dateto = -1)
{
	if($datefrom<=0) { return "Länge sedan"; }
	if($dateto==-1) { $dateto = time(); }

	$difference = $dateto - $datefrom;

	if($difference < 60)
		$interval = "s";
	else if($difference >= 60 && $difference<60*60)
		$interval = "n";
	else if($difference >= 60*60 && $difference<60*60*24)
		$interval = "h";
	else if($difference >= 60*60*24 && $difference<60*60*24*7)
		$interval = "d";
	else if($difference >= 60*60*24*7 && $difference < 60*60*24*30)
		$interval = "ww";
	else if($difference >= 60*60*24*30 && $difference < 60*60*24*365)
		$interval = "m";
	else if($difference >= 60*60*24*365)
		$interval = "y";

	switch($interval)
	{
		case "m":
			$months_difference = floor($difference / 60 / 60 / 24 / 29);
			while (mktime(
				date("H", $datefrom), date("i", $datefrom),
				date("s", $datefrom), date("n", $datefrom)+($months_difference),
				date("j", $dateto), date("Y", $datefrom)) < $dateto)
				$months_difference++;

			$datediff = $months_difference;

			if($datediff==12)
				$datediff--;

			$res = ($datediff==1) ? "$datediff månad sedan" : "$datediff månader sedan";
			break;

		case "y":
			$datediff = floor($difference / 60 / 60 / 24 / 365);
			$res = ($datediff == 1) ? "I fjol" : "$datediff år sedan";
			break;

		case "d":
			$datediff = floor($difference / 60 / 60 / 24);
			$res = ($datediff == 1) ? "Igår" : "$datediff dagar sedan";
			break;

		case "ww":
			$datediff = floor($difference / 60 / 60 / 24 / 7);
			$res = ($datediff == 1) ? "Förra veckan" : "$datediff veckor sedan";
			break;

		case "h":
			$datediff = floor($difference / 60 / 60);
			$res = ($datediff == 1) ? "En timme sedan" : "$datediff timmar sedan";
			break;

		case "n":
			$datediff = floor($difference / 60);
			$res = ($datediff == 1) ? "$datediff minut sedan" : "$datediff minuter sedan";
			break;

		case "s":
			$datediff = $difference;
			$res = ($datediff == 1) ? "$datediff sekund sedan" : "$datediff sekunder sedan";
			break;
	}

	return $res;
}

