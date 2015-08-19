<?php

error_reporting(E_ALL);
ini_set('display_errors', 1);
    if (session_status() === PHP_SESSION_NONE){session_start();}
    $loginAuthorised = isset($_SESSION['auth']) ? ($_SESSION['auth'] == 1) : false;

    if(!$loginAuthorised)
        header("Location: index.php?status=logout");  

    $page = isset($_GET['page']) ? $_GET['page'] : false;
    
    if($page)
        $contentFile = 'explore.php';
    else
        $contentFile = 'userProfile.php';

?>
<html>
<head><title>cs290 Final Project</title></head>
<link href='http://fonts.googleapis.com/css?family=Josefin+Slab' rel='stylesheet' type='text/css'>
<link rel="stylesheet" type="text/css" href="style_loggedin.css">
<script type="text/javascript" src="http://ajax.googleapis.com/ajax/libs/jquery/1.11.3/jquery.min.js"></script>
<script type="text/javascript" src="script.js"></script>

<div id="loginErrorDiv"></div>    
    
<body>
    <div id='container'>
        <div id="topNav">
            <?php
                 echo 'Hello, ' . $_SESSION['username'] . '! | ';
            ?>
            <a id='exploreLink' href="home.php?page=explore">Explore</a> | <a href="home.php">myCard</a> | <a href="index.php?status=logout"> Log Out</a>
        </div>

        <div id="rightContent">
            <?php include_once($contentFile) ?>
        </div>
    </div>
</body>
</html>