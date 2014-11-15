<?php
	if($dir = opendir("."))
	{
		while(false !== ($file = readdir($dir)))
		{
			if ($file != "." && $file != ".." && $file != "update.php")
			{
				$md5 = md5_file($file);
				echo("$file\t$md5\n");
			}
		}
		
		closedir($dir);
	}
?>