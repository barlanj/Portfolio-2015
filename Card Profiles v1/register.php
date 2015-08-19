<?php

    include_once('db_connect.php');
    $username_retrieved = null;
    $username = isset($_POST['r_username']) ? $mysqli->real_escape_string($_POST['r_username']) : null;
    $password = isset($_POST['r_password']) ? $mysqli->real_escape_string($_POST['r_password']) : null;

    $msg = array();

    /* Validation */

        if(strlen($password) == 0 || $password == "" || $password == null)
            $msg[] = 'Password is missing ';
        else {
            if (strlen($password) < 7)
                $msg[] = 'Password must have at least 7 characters.';
            
            if (!preg_match('`[A-Z]`', $password))
                $msg[] = 'Password must have at least one capital letter.';
        }
          
        if($username == null)
            $msg[] = 'Username is missing.';
        else {
            if(strlen($username) < 5)
               $msg[] = 'Username must have at least 5 character.';
            if(!ctype_alnum($username))
               $msg[] = 'Username accepts only A-Z, a-z, 0-9.';
        }
  
        if (empty($msg)) {

            //check if the username already exist
            $query = "SELECT username FROM final_userLogin WHERE username = '$username'";
            $result = $mysqli->query($query);
            if($result->num_rows > 0)
               $msg[] = 'Username is already used. Try another.';
            $result->close();
            
            //if no errors, then attempt to insert into table
            if(empty($msg)) {
                $password = md5($password);
                $queryInsert =  "INSERT INTO final_userLogin (username, password)".
                                "VALUES ('$username', '$password')";
                $result = $mysqli->query($queryInsert);
                if( !$result )
                        $msg[] = "Unable to add Item. ErrNo:". $mysqli->errno;
                else {
                    if (session_status() === PHP_SESSION_NONE){session_start();}
                    //create a card
                    $userid = null;

                    //find the userid in the database table
                    $user_query = "SELECT ID FROM final_userLogin WHERE username = '$username'";
                    $result = $mysqli->query($user_query);
                    if($result->num_rows > 0) {
                        $row = $result->fetch_array(MYSQL_ASSOC);
                        $userid = $row['ID'];
                    } else {
                        $userid = null;
                    }
                    $result->close();
                    
                    //create new card data using ID
                    $query =  "INSERT INTO final_userCards (userID, fullname) VALUES ('$userid', '$fullname')";
                    $result = $mysqli->query($query);
                    
                    
                    //successful registration
                    $_SESSION['auth'] = 1;
                    $_SESSION['username'] = $username;
                    $_SESSION['userIP'] = $_SERVER['REMOTE_ADDR'];
                    //header("Location: index.php");
                    $msg[] = 'successful registration';
                }
            }
        }  


        echo json_encode($msg);
?>