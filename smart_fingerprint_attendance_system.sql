-- phpMyAdmin SQL Dump
-- version 5.2.1
-- https://www.phpmyadmin.net/
--
-- Host: 127.0.0.1
-- Generation Time: Sep 12, 2025 at 12:42 AM
-- Server version: 10.4.32-MariaDB
-- PHP Version: 8.0.30

SET SQL_MODE = "NO_AUTO_VALUE_ON_ZERO";
START TRANSACTION;
SET time_zone = "+00:00";


/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8mb4 */;

--
-- Database: `smart_fingerprint_attendance_system`
--

-- --------------------------------------------------------

--
-- Table structure for table `employee_attendance`
--

CREATE TABLE `employee_attendance` (
  `emp_ID` int(11) NOT NULL,
  `emp_name` varchar(100) NOT NULL,
  `finger_ID` int(11) NOT NULL,
  `in_time` datetime DEFAULT NULL,
  `out_time` datetime DEFAULT NULL,
  `duration` time DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

--
-- Dumping data for table `employee_attendance`
--

INSERT INTO `employee_attendance` (`emp_ID`, `emp_name`, `finger_ID`, `in_time`, `out_time`, `duration`) VALUES
(1, 'Raad', 101, '2025-09-10 09:00:00', '2025-09-10 17:00:00', NULL),
(2, 'Arif', 102, '2025-09-10 10:15:00', '2025-09-10 18:00:00', NULL),
(3, 'Raiyan', 103, '2025-09-10 08:45:00', '2025-09-10 17:15:00', NULL),
(4, 'Rony', 104, '2025-09-10 09:30:00', '2025-09-10 18:10:00', NULL),
(5, 'Zabed', 105, '2025-09-10 08:50:00', '2025-09-10 16:40:00', NULL),
(6, 'Shuvo', 106, '2025-09-10 09:10:00', '2025-09-10 17:45:00', NULL),
(7, 'Sagar', 107, '2025-09-10 10:00:00', '2025-09-10 19:00:00', NULL),
(8, 'Sudad', 108, '2025-09-10 08:55:00', '2025-09-10 16:55:00', NULL),
(9, 'Dipu', 109, '2025-09-10 09:20:00', '2025-09-10 17:50:00', NULL),
(10, 'Zobayer', 110, '2025-09-10 09:05:00', '2025-09-10 17:05:00', NULL),
(11, 'Aurko', 111, '2025-09-10 09:40:00', '2025-09-10 18:20:00', NULL);

--
-- Indexes for dumped tables
--

--
-- Indexes for table `employee_attendance`
--
ALTER TABLE `employee_attendance`
  ADD PRIMARY KEY (`emp_ID`,`finger_ID`);
COMMIT;

/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
