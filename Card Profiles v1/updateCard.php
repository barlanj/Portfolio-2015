<?php
    if (session_status() === PHP_SESSION_NONE){session_start();}
    include_once('db_connect.php');
    
    $loginAuthorised = isset($_SESSION['auth']) ? ($_SESSION['auth'] == 1) : false;

    if(!$loginAuthorised)
        header("Location: index.php?status=logout");



    //find the person's id
    $username = isset($_SESSION['username']) ? $_SESSION['username'] : null;
    $userid = null;

    $user_query = "SELECT ID FROM final_userLogin WHERE username = '$username'";
    $result = $mysqli->query($user_query);
    if($result->num_rows > 0) {
        $row = $result->fetch_array(MYSQL_ASSOC);
        $userid = $row['ID'];
    } else {
        $userid = null;
    }
    $result->close();

    //collect POST data
    $fullname = isset($_POST['fullname']) ? $mysqli->real_escape_string($_POST['fullname']) : null;
    $title = isset($_POST['title']) ? $mysqli->real_escape_string($_POST['title']) : null;
    $address = isset($_POST['address']) ? $mysqli->real_escape_string($_POST['address']) : null;
    $email = isset($_POST['email']) ? $mysqli->real_escape_string($_POST['email']) : null;
    $linkedin = isset($_POST['linkedin']) ? $mysqli->real_escape_string($_POST['linkedin']) : null;
    $github = isset($_POST['github']) ? $mysqli->real_escape_string($_POST['github']) : null;
    $back = isset($_POST['back']) ? $mysqli->real_escape_string($_POST['back']) : null;
    $share = isset($_POST['share']) ? $mysqli->real_escape_string($_POST['share']) : null;


    function updateCard($name, $value, $mysqli, $id) {
        $query = "UPDATE final_userCards SET $name='$value' WHERE userID='$id' LIMIT 1";
        $result = $mysqli->query($query);
    }
    
    //update based on what was received
    if($fullname)
        updateCard('fullname', $fullname, $mysqli, $userid);
    if($title) 
        updateCard('title', $title, $mysqli, $userid);
    if($address)
        updateCard('address', $address, $mysqli, $userid);
    if($email)
        updateCard('email', $email, $mysqli, $userid);
    if($linkedin)
        updateCard('linkedin', $linkedin, $mysqli, $userid);
    if($github)
        updateCard('github', $github, $mysqli, $userid);
    if($back)
        updateCard('back', $back, $mysqli, $userid);
    if($share == 0 || $share == 1)
        updateCard('public', $share, $mysqli, $userid);


/*
    //update entry
    $query = "UPDATE final_userCards SET fullname='$fullname',
                title='$title',
                address='$address',
                email='$email',
                linkedin='$linkedin',
                github='$github' 
                WHERE userID='$userid' LIMIT 1";
    $result = $mysqli->query($query);
*/
?>