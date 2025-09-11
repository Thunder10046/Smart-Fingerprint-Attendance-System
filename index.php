<?php
$servername = "localhost";
$username   = "root";      
$password   = "";          
$dbname     = "smart_fingerprint_attendance_system";  


$conn = new mysqli($servername, $username, $password, $dbname);

if ($conn->connect_error) {
    die("Connection failed: " . $conn->connect_error);
}

$sql = "
    SELECT 
        emp_ID,
        emp_name,
        finger_ID,
        in_time,
        out_time,
        TIMEDIFF(out_time, in_time) AS duration
    FROM Employee_Attendance
    ORDER BY emp_ID ASC
";
$result = $conn->query($sql);
?>

<!DOCTYPE html>
<html>
<head>
    <title>Employee Attendance</title>
    <style>
        body {
            font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
            background: #f4f6f9;
            margin: 0;
            padding: 20px;
        }
        h2 {
            text-align: center;
            margin-bottom: 20px;
            color: #333;
        }
        .table-container {
            background: #fff;
            border-radius: 12px;
            padding: 20px;
            box-shadow: 0 4px 12px rgba(0,0,0,0.1);
            max-width: 1000px;
            margin: auto;
        }
        table {
            border-collapse: collapse;
            width: 100%;
            border-radius: 12px;
            overflow: hidden;
        }
        th, td {
            padding: 12px 15px;
            text-align: center;
        }
        th {
            background: #007bff;
            color: white;
            font-weight: 600;
        }
        tr:nth-child(even) {
            background: #f9fbfd;
        }
        tr:hover {
            background: #eef5ff;
        }
        td {
            color: #333;
        }
    </style>
</head>
<body>

<h2>Smart Fingerprint Attendace System Dashboard</h2>

<div class="table-container">
    <table>
        <tr>
            <th>Employee ID</th>
            <th>Employee Name</th>
            <th>Finger ID</th>
            <th>In Time</th>
            <th>Out Time</th>
            <th>Duration</th>
        </tr>
        <?php
        if ($result->num_rows > 0) {
            while ($row = $result->fetch_assoc()) {
                echo "<tr>
                        <td>{$row['emp_ID']}</td>
                        <td>{$row['emp_name']}</td>
                        <td>{$row['finger_ID']}</td>
                        <td>{$row['in_time']}</td>
                        <td>{$row['out_time']}</td>
                        <td>{$row['duration']}</td>
                      </tr>";
            }
        } else {
            echo "<tr><td colspan='6'>No records found</td></tr>";
        }
        ?>
    </table>
</div>

</body>
</html>

<?php
$conn->close();
?>
