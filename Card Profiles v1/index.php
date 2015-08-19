<?php

    ini_set('display_errors', 'On');

    if (session_status() === PHP_SESSION_NONE){session_start();}
    $status = isset($_GET['status']) ? $_GET['status'] : null;
    $loginAuthorised = isset($_SESSION['auth']) ? $_SESSION['auth'] : null;

    if(isset($status) && $status == 'logout') {
        session_destroy();
        $loginAuthorised = false;
        header("Location: index.php");
    } else {
        $loginAuthorised = isset($_SESSION['auth']) ? ($_SESSION['auth'] == 1) : false; 
    }

    if($loginAuthorised) {
        header("Location: home.php");
    } 
?>



<html>
<head>
    <title>cs290 Final Project</title>
    <link rel="stylesheet" type="text/css" href="index.css">
    <script type="text/javascript" src="http://ajax.googleapis.com/ajax/libs/jquery/1.11.3/jquery.min.js"></script>
    <script type="text/javascript" src="script.js"></script>

</head>

    
<body>
     
<div id="loginErrorDiv"></div>
    
<div id='mainContainer'>
    <div id='login'>
        <h1>Log In</h1>
        <div class="formContainer">
            <form id="loginForm" action="login.php" method="POST">
            <input type="text" name="username" id="username" value="username"/><br />
            <input type="password" name="password" id="password" value="password"/><br />
            <button id="loginButton" name="loginButton">Login</button>
            </form>
        </div> 

    </div>

    <div id='register'>
        <h1>Register</h1>
        <div class="formContainer">
            <form id="registerForm" action="register.php" method="POST">
            <input type="text" name="r_username" id="r_username" value="username"/><br />
            <input type="password" name="r_password" id="r_password" value="password"/><br />
            <button type='submit' id="registerButton">Register</button>
            </form>
        </div> 
    </div>
</div>    



</body>
</html>