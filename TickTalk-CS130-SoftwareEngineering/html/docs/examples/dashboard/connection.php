<?php 
/*$con=mysql_connect('ec2-52-27-111-35.us-west-2.compute.amazonaws.com',"","");  
if (!$con)
{
    die('Could not connect: ' . mysql_error());
} */  



$servername = "ec2-52-27-111-35.us-west-2.compute.amazonaws.com";
//$username = "username";
//$password = "password";

// Create connection
$con = new mysqli($servername/*, $username, $password*/);

// Check connection
if (!$con) {
    die("Connection failed: " . mysqli_connect_error());
}
echo "Connected successfully";
?>