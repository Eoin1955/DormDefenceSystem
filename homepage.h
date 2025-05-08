String homePagePart1 = F(R"=====(<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width, initial-scale=1, viewport-fit=cover">
  <title>Temperature Reading WebServer</title>
  <style>
    body {       
      background-color: DodgerBlue;
    }
    .flex-Container {
      display: flex;
      flex-direction: column;
      background-color: DodgerBlue;
      align-items: center;
    }
    h1 {
      font: bold;
      font-size: 40px;
      font-family: Arial;
      color: black;
      text-align: center;
    }
    p {
      font-size: 25px;
      font-family: Arial;
      color: black;
      text-align: center;
    }
    th, td {
      font-size: 25px;
      padding: 8px;
      text-align: left;
      border-bottom: 1px solid #ddd;
    }
  </style>
  <script>
    function fetchUltrasonic() {
      fetch('/Ultrasonic') // Call the ESP32 server
        .then(response => response.text()) // Convert response to text
        .then(sonic => {
          console.log("Ultrasonic:", sonic); // Debugging output in console
          document.getElementById("UltrasonicValue").innerText = sonic; // Update webpage
          doorStatus(parseFloat(sonic)); // Determine if door is open/closed
        })
        .catch(error => console.error('Error fetching Ultrasonic:', error)); // Handle errors
    }

    function doorStatus(distance) {
      const openEl = document.getElementById("Open");
      const closedEl = document.getElementById("Closed");

      if (distance < 15) {
        closedEl.style.display = "none";
        openEl.style.display = "block";  
        openEl.style.background = "green";
      } else {
        openEl.style.display = "none";
        closedEl.style.display = "block";  
        closedEl.style.background = "red";
      }
    }

    window.onload = () => {
      fetchUltrasonic(); // Initial fetch
      document.getElementById("Open").style.display = "none";
      document.getElementById("Closed").style.display = "none";
      setInterval(fetchUltrasonic, 1000); // Repeat every second
    };
  </script>
</head>
<body>
  <div class="container">
    <h1>Dorm Defence System</h1>
    <p>Distance: <span id="UltrasonicValue">Loading...</span> cm</p>
    <p id="Open"><b>Door status: Open</b></p>
    <p id="Closed"><b>Door status: Closed</b></p>
  </div>
</body>

</html>)=====");
