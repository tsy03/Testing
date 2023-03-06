<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta http-equiv="X-UA-Compatible" content="IE=edge">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>About us</title>
    <!----<link rel="stylesheet" type="text/css" href="about us (iwp project).css">--->
    <style>
      *{
    margin: 0;
    padding: 0;
    box-sizing: border-box;
    font-family: Arial, Helvetica, sans-serif;
    list-style: none;
    text-decoration: none;
  }
  
  .wrapper11{
    width: 100%;
    min-height: 100vh;
    overflow: hidden;
    background-color: rgb(183, 178, 178);
    margin-top: 100px;
  }
  
  .row{
    width: 100%;
    padding: 60px 100px;
  }
  
  .image-section{
    width: 30%;
    float: left;
  }
  
  .image-section img{
    width: 100%;
    height: auto;
  
  }
  
  .content{
    width: 60%;
    float: right;
    margin-left: 50px;
  
  }
  
  .content h2{
    font-family: sans-serif;
    color: #b60112;
    margin-top: 25px;
    font-size: 40px;
    text-transform: uppercase;
  }
  
  .content p{
    font-size: 20px;
    font-family: serif;
    margin-top: 10px;
    line-height: 1.5;
  }
  
  .wrapper1{
    width: 100%;
    min-height: 100vh;
    background-color: rgb(213, 206, 206);
  }
  
  .row1{
    width: 100%;
    padding: 50px 100px;
  }
  
  .image-section2{
    width: 30%;
    float: right;
  }
  
  .image-section2 img{
    width: 100%;
    height: auto;
  
  }
  
  .content2{
    width: 60%;
    float: left;
    margin-left: 50px;
  
  }
  
  .content2 p{
    font-size: 20px;
    font-family: serif;
    margin-top: 10px;
    line-height: 1.5;
  }
  
  .wrapper2{
    width: 100%;
    min-height: 100vh;
    background-color: rgb(183, 178, 178);
  }
  
  .row2{
    width: 100%;
    padding: 50px 100px;
  }
  
  .image-section3{
    margin-top: 100px;
    display: flex;
    align-items: center;
    justify-content: center;
    width: 100%;
    height: 300px;
  }
  
  .image-section3 img{
    width: 70%;
    height: auto;
    margin-left: 30px;
  
  }
  
  .content3{
    width: 100%;
    margin-top: auto;
  
  }
  
  .content3 p{
    font-size: 20px;
    font-family: serif;
    margin-left: 50px;
    margin-top: 90px;
    text-align: justify;
    line-height: 1.5;
  }
  
  @media screen and (max-width: 768px){
    .image-section{
      width: 100%;
      float: none;
    }
    .content{
      width: 100%;
      float: none;
      margin-left: 0px;
    }
  }
  

  
  .logo img{
    margin-left: 50px;
    width: 80px;
    height: 66px;
  }
  
  .navmenu{
    display: flex;
  }
  
  .navmenu a{
    font-weight: bold;
    color: #2F3841;
    font-size: 20px;
    text-transform: capitalize;
    padding: 10px 20px;
  }
  
  .navmenu a:hover{
    color: red;
    text-decoration: underline;
  }
  
  .help{
    display: flex;
    align-items: center;
    margin-left: 50%;
    margin-right: 70px;
    cursor: pointer;
  }
  
  .help a{
    margin-right: 20px;
    font-weight: bold;
    font-size: 20px;
    text-transform: capitalize;
    color: #2F3841;
  }
  
  .help a:hover{
    color: red;
    text-decoration: underline;
  }
  
  .head .action a{
    cursor: pointer;
    color: #2F3841;
  }
  
  .head .action a:hover{
    color: red;
  }
  
  .head .action a{
    cursor: pointer;
    color: #2F3841;
  }
  
  .head .action a:hover{
    color: red;
  }
  
  .wltemplate{
    padding: 150px 50px;
    width: 100%;
    height: 100%;
  }
  
  .wltemplate .head{
    display: flex;
    position: fixed;
    width: 100%;
    top: 91px;
    right: 0;
    padding-left: 50px;
    height: fit-content;
    background-color: white;
    background-position: 10px;
    align-items: center;
    justify-content: space-between;
    border-bottom : 1px solid gray;
  }
  
  .wltemplate .head .heading{
    font-size: 50px;
    font-family: 'Open Sans';
    font-weight: 700;
    color: #2F3841;
  
  }
  
  .wltemplate .head .action{
    margin-top: 0px;
    margin-right: 100px;
    text-decoration: none;
    border-bottom: 5px solid;
    font-size: 30px;
    font-family: 'Open Sans';
    font-weight: 700;
    color: black;
  }
  
  .first-info .companyname{
    font-weight: bold;
    font-family: Arial, Helvetica, sans-serif;
    font-size: 20px;
  }
  
  .first-info .companyaddress{
    font-size: 15px;
  }
  
  .first-info a{
    color: #2F3841;
    font-weight: bold;
    cursor: pointer;
    
  }
  
  .first-info a:hover{
    color: red;
    text-decoration: underline;
  }
  
  .second-info a{
    color: #2F3841;
    font-weight: bold;
    cursor: pointer;
  }
  
  .second-info a:hover{
    color: red;
    text-decoration: underline;
  }
  
  .third-info a{
    color: #2F3841;
    font-weight: bold;
    cursor: pointer;
  }
  
  .third-info a:hover{
    color: red;
    text-decoration: underline;
  }
  
  .fourth-info a{
    color: #2F3841;
    font-weight: bold;
    cursor: pointer;
  }
  
  .fourth-info a:hover{
    color: red;
    text-decoration: underline;
  }
  
  .social-icon img{
    cursor: pointer;
  }
  
  .social-icon img:hover{
    background-color: red;
  }
    </style>
</head>
<body>
  <header>
    <?php include("Header.php"); ?>
  </header>
  <section class="wltemplate">
    <div class="head">
      <h2 class="heading"><img src="image/help-circle-regular-24.png" style="width: 50px; height: 50px; float: left; margin-top: 4px;"><t>ABOUT US</t></h2>
            
      <h4 class="action"><a href="http://localhost/Project/landing.php"><img src="image/arrow-back-regular-24.png" style="width: 30px; height: 30px; float: left; margin-top: 4px;">Back</a></h4>
    </div>

    <div class="wrapper11">
      <div class="row">
        <div class="image-section">
          <img src="image/about us.jpg" style="height: 450px;">
        </div>
        <div class="content">
          <h2>our company</h2>
          <p>F&T Cloth is a leading fashion brand that has been at the forefront of the industry for many years. With a deep love for fashion and a commitment to quality, we strive to bring you the latest styles and trends, while also ensuring that our products are made with the highest level of craftsmanship.</p>

          <p>Our team of talented designers and tailors work tirelessly to create collections that are not only beautiful but also practical, offering a wide range of clothing and accessories for any occasion. Whether you're looking for a stylish outfit for a special event, comfortable everyday wear, or eye-catching accessories, we have something for everyone. Our commitment to innovation and quality is evident in every piece of clothing and accessory we offer, and we take great pride in knowing that our customers love and trust the F&T Cloth brand.</p>
        </div>
      </div>
    </div>
    <div class="wrapper1">
      <div class="row1">
        <div class="image-section2">
          <img src="image/about us 2.jpg" style="height: 450px;">
        </div>
        <div class="content2">
          <p>At F&T Cloth, we understand that everyone is unique and has their own personal style. That's why we offer a wide range of products in various sizes, colors, and styles, so that every customer can find something that suits their individual taste and preferences. Our online store is designed to make shopping easy and convenient, and our physical store provides a warm and welcoming shopping environment, where our knowledgeable staff is always on hand to assist you.</p>

          <p>We believe in providing our customers with the best shopping experience possible, and this is why we place such a strong emphasis on customer service. Our team is always available to answer any questions you may have, help you find the perfect outfit, or offer advice on the latest fashion trends. Whether you prefer to shop in-store or online, we are here to provide you with an enjoyable and seamless shopping experience.</p>
        </div>
      </div>
    </div>
    <div class="wrapper2">
      <div class="row2">
        <div class="image-section3">
          <img src="image/Clothing-Store.jpg" style="height: 450px;">
        </div>
        <div class="content3">
          <p>In addition to our focus on style and quality, we are also deeply committed to sustainability and ethical sourcing. We believe that fashion should not come at the cost of the environment or the people who make our products, and we work hard to ensure that our supply chain is transparent and responsible. This means that our customers can feel confident that they are not only buying beautiful and well-made clothing, but also making a positive impact on the world.</p>

          <p>So whether you're a fashion-forward trendsetter, a busy professional, or just looking for a new outfit, come and visit us at F&T Cloth. With our focus on style, comfort, affordability, and sustainability, we are dedicated to providing our customers with the best clothing and accessory experience possible. We guarantee that you'll love what you find! Our passion for fashion and commitment to quality is evident in everything we do, and we look forward to helping you find your perfect outfit.</p>
        </div>
      </div>
    </div>
  </section>

  <section>
      <?php include("footer.php"); ?>
  </section>

</body>
</html>
