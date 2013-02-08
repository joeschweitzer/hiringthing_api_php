<!DOCTYPE html>
<?php

// Override this with your credentials!
$credentials = array(
  'subdomain' => 'yourdomain',
  'api_key' => 'yourapikey',
  'api_password' => 'yourapipassword'
);

require_once('hiringthing.inc');

$hiringthing = new HiringThing($credentials);
?>
<html>
<head>
  <title>HiringThing API example</title>
  <style>
table, td {
  border: 1px solid black;
  padding: 0;
  margin: 0;
}
td {
  padding: 5px;
}
thead td {
  background-color: black;
  color: white;
  font-weight: bold;
}
  </style>
</head>
<body>

<h2>Jobs</h2>

<table cellpadding="0" cellspacing="0">
<thead>
  <tr>
  <td>ID</td>
  <td>Title</td>
  <td>Abstract</td>
  <td>Archived</td>
  </tr>
</thead>
<tbody>
<?php foreach ($hiringthing->getJobs() as $job): ?>
  <tr>
  <td><?php print $job->id ?></td>
  <td><?php print $job->title ?></td>
  <td><?php print $job->abstract ?></td>
  <td><?php print $job->archived ? 'Yes' : 'No' ?></td>
<?php endforeach; ?>
</tbody>
</table>

<h2>Applications (rated)</h2>

<table cellpadding="0" cellspacing="0">
<thead>
  <tr>
  <td>ID</td>
  <td>Job</td>
  <td>Name</td>
  <td>Rating</td>
  <td>Archived</td>
  </tr>
</thead>
<tbody>
<?php foreach ($hiringthing->getApplications('rated') as $application): ?>
  <tr>
  <td><?php print $application->id ?></td>
  <td><?php print $application->job ?></td>
  <td><span class="name"><?php print $application->first_name ?> <?php print $application->last_name ?></span></td>
  <td><?php print $application->rating ?></td>
  <td><?php print $application->archived ? 'Yes' : 'No' ?></td>
  </tr>
<?php endforeach; ?>
</tbody>
</table>

<?php
$active_jobs = $hiringthing->getJobs('active');
$job = $active_jobs[0];
?>
<h2>Applications (unrated) for <?print $job->title; ?> (ID: <?print $job->id; ?>)</h2>
<table cellpadding="0" cellspacing="0">
<thead>
  <tr>
  <td>ID</td>
  <td>Job</td>
  <td>Name</td>
  <td>Archived</td>
  </tr>
</thead>
<tbody>
<?php foreach ($job->getApplications('unrated') as $application): ?>
  <tr>
  <td><?php print $application->id ?></td>
  <td><?php print $application->job ?></td>
  <td><?php print $application->first_name ?> <?php print $application->last_name ?></td>
  <td><?php print $application->archived ? 'Yes' : 'No' ?></td>
  </tr>
<?php endforeach; ?>
</tbody>
</table>

</body>
</html>
