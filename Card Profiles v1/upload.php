<?php
    //prevent direct access to this page if not logged in
    if (session_status() === PHP_SESSION_NONE){session_start();}
    include_once('db_connect.php');
    
    $loginAuthorised = isset($_SESSION['auth']) ? ($_SESSION['auth'] == 1) : false;

    if(!$loginAuthorised)
        header("Location: index.php?status=logout");


    $msg = array();

    if(isset($_POST['fileUploadButton']))
    {

        //file properties of uploaded file
        $name = $_FILES['photoUpload']['name'];
        $type = $_FILES['photoUpload']['type'];
        $size = $_FILES['photoUpload']['size'];
        $temp = $_FILES['photoUpload']['tmp_name']; //temporary storage area
        $error  = $_FILES['photoUpload']['error'];

        //
        if ($error > 0) {
            $msg[] = "Error uploading file. Code $error";
        } else {

            $allowed_types = array("image/jpeg", "image/gif", "image/png");
            if(!in_array($type, $allowed_types) || $size > 1500000) { 
                $msg[] = "Format is not allowed, or file size too big!";
            }
    
            if (empty($msg)) {
                //update the pseron in user table
                $username = isset($_SESSION['username']) ? $_SESSION['username'] : null;
                $userid = null;
                $data_id = null;

                $user_query = "SELECT ID FROM final_userLogin WHERE username = '$username'";
                $result = $mysqli->query($user_query);
                if($result->num_rows > 0) {
                    $row = $result->fetch_array(MYSQL_ASSOC);
                    $userid = $row['ID'];
                } else {
                    $userid = null;
                }

                //find same person in data table
                $user_query = "SELECT * FROM final_userCards WHERE userID='$userid' LIMIT 1";
                $result = $mysqli->query($user_query);
                if($result->num_rows > 0) {
                    $row = $result->fetch_array(MYSQL_ASSOC);
                    $data_id = $row['data_id'];
                } 

                define ('SITE_ROOT', realpath(dirname(__FILE__)));
                $dest = SITE_ROOT . "/uploaded/$data_id" . "_" . basename($_FILES['photoUpload']['name']);
                if (move_uploaded_file($temp, $dest)) {
                    chmod($dest, 0755);
                    $msg[] = 'Upload was successful';
                    //$msg[] = $data_id;
                    header("Location: home.php");
                } else {
                    $msg[] = "error. could not move to directory.";
                }  

                $fileDir = strstr($dest, "/uploaded");
                $query = "UPDATE final_userCards SET imageName='$fileDir' WHERE userID='$userid' LIMIT 1";
                $result = $mysqli->query($query); 
            }
       
        } 
    }

if(empty($msg)) {
    echo "Change your profile picture!";
} else {
    echo json_encode($msg);
}


?>

<form enctype="multipart/form-data"  action="upload.php" method="post">
    <input type="hidden" name="MAX_FILE_SIZE" value="1500000" />
    <input type="file" name="photoUpload" id="photoUpload">
    <input type="submit" name="fileUploadButton" id="fileUploadButton" value="Upload">
</form>
