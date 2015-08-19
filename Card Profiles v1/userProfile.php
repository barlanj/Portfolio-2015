<?php
    include_once('db_connect.php');

    if (session_status() === PHP_SESSION_NONE){session_start();}
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


    $query = "SELECT * FROM final_userCards WHERE userID='$userid'";
    $result = $mysqli->query($query);
    $row = $result->fetch_array(MYSQL_ASSOC);

    $fullname = ($row['fullname'] != null) ? $row['fullname'] : "Full Name";
    $title = ($row['title'] != null) ? $row['title'] : "job title";
    $address = ($row['address'] != null) ? $row['address'] : "City, State";
    $email = ($row['email'] != null) ? $row['email'] : "email@addrss.com";
    $linkedin = ($row['linkedin'] != null) ? $row['linkedin'] : "your_username";
    $github = ($row['github'] != null) ? $row['github'] : "your_username";
    $back = ($row['back'] != null) ?  $row['back'] : "About yourself...";
    $public = ($row['public'] == 1) ? "checked" : "";
    $photo = "http://web.engr.oregonstate.edu/~barlanj/final" . $row['imageName'];


?>
<div class='paragraph'>
    Your profile card is set to
    <div class="onoffswitch">
        <?php echo "<input type='checkbox' name='onoffswitch' class='onoffswitch-checkbox' id='myonoffswitch' $public />" ?>
        <label class="onoffswitch-label" for="myonoffswitch">
            <span class="onoffswitch-inner"></span>
            <span class="onoffswitch-switch"></span>
        </label>
    </div>
</div>

<div id='uploadContainer'>
    <?php include_once('upload.php') ?>
</div>

<div class="cardContainer">
    <div class="userCard">
        <div class="frontFace">
            <div class="card_left">
                <?php echo "<div class='userCard_photo' style='background-image:url($photo);background-repeat: no-repeat;background-size: cover;'></div>"; ?>
                <div class="userCard_fullname"><input type="text" name="fullname" id="fullname" class="cardInput" value="<?php echo $fullname?>" /></div>
                <div class="userCard_title"><input type="text" name="title" id="title" class="cardInput" value="<?php echo $title?>"/></div>
            </div>

            <div class="card_right">
                <div class="userCard_address"><input type="text" name="address" id="address" class="cardInput" value="<?php echo $address?>"/></div>
                <div class="userCard_email"><input type="text" name="email" id="email" class="cardInput" value="<?php echo $email?>"/></div>

                <div class="userCard_links">
                    <div class="userCard_link_linkedin"><div class="icon_container"><img src="site_images/icon_linkedin.png" class='icon_linkedin'></div><input type="text" name="linkedin" id="linkedin" class="cardInput" value="<?php echo $linkedin?>"/></div>
                    <div class="userCard_link_github"><div class="icon_container"><img src="site_images/icon_github.png" class='icon_github'></div><input type="text" name="github" id="github" class="cardInput" value="<?php echo $github?>"/></div>
                </div>
            </div>
        </div>


        <div class="backFace">
            <div class="userCard_back"> <textarea class="cardInput" id="back" name="back"><?php echo $back?></textarea></div>
        </div>
    </div>

</div>



