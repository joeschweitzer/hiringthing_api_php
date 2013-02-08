### HiringThing API for PHP

This library allows you to access your [HiringThing](http://www.hiringthing.com) account via the HiringThing [API](http://www.hiringthing.com/api/index.html).

This is the READ_ONLY version. For write access, please contact [support@hiringthing.com](mailto:support@hiringthing.com)

This library isn't dependent on any other libraries and by default it supports PHP 5.2 and above. However, it's designed to integrate into any framework and could be made to work even with PHP 5.0 and above. See the "Portability" section at the end for more details.

### Installation

Copy the hiringthing.inc file somewhere that your application can access it.

Then include it with:

```php
include_once('hiringthing.inc');
```

If you're using Drupal, check out the [HiringThing module for Drupal](http://drupal.org/project/hiringthing).

### Configuration

Once you've obtained an api_key and password from your HiringThing account, you can create a HiringThing object as follows:

```php
$hiringthing = new HiringThing(array(
  'api_key' => 'key',
  'api_password' => 'pass',
  'subdomain' => 'yoururl' // from http://yoururl.hiringthing.com
));
```

Be sure that your api is enabled in the "Account Details" section of your HiringThing account. 

### Usage

```php
// get all your jobs
$myjobs = $hiringthing->getJobs();

// get a specific job
$myjob = $hiringthing->getJob(111);

// get active jobs
$myjobs = $hiringthing->getJobs('active');

// get hidden jobs
$myjobs = $hiringthing->getJobs('hidden');

// get archived jobs
$myjobs = $hiringthing->getJobs('archived');

// access applications on a specific job
$myjob = $hiringthing->getJob(111);
$myjob->getApplications();

// get all applications with no rating for this job
$myjob->getApplications('unrated');

// get all applications with a rating for this job
$myjob->getApplications('rated');

// get all archived applications for this job
$myjob->getApplications('archived');

// get a specific application 
$myapplication = $hiringthing->getApplication(111);

// get all applications
$myapplications = $hiringthing->getApplications();

// get all applications with a rating
$myapplications = $hiringthing->getApplications('rated');

// get all applications with no rating
$myapplications = $hiringthing->getApplications('unrated');

// get all archived applications 
$myapplications = $hiringthing->getApplications('archived');
```

Application objects have the following attributes:

```php
// Application objects

	$myapplication->id // app id
	$myapplication->job // job title
	$myapplication->job_id 
	$myapplication->status 
	$myapplication->first_name 
	$myapplication->last_name 
	$myapplication->phone 
	$myapplication->email 
	$myapplication->rating 
	$myapplication->applied_at 
	$myapplication->source // where the applicant came from 
	$myapplication->archived // 1 for archived, 0 for active
```

Job objects have these attributes:

```php
// Job objects

	$myjob->id // job id
	$myjob->company // company name
	$myjob->job_code 
	$myjob->title 
	$myjob->abstract 
	$myjob->description 
	$myjob->city 
	$myjob->state 
	$myjob->country 
	$myjob->archived // 1 for archived, 0 for active
	$myjob->url // short url
	$myjob->created_at 
```

[More documentation](http://www.hiringthing.com/api/index.html) is available on the [HiringThing website](http://www.hiringthing.com).

### Portability

HTTP requests are made by a sub-class of the `HiringThingHttpRequester` class.

A default sub-class (named `HiringThingHttpRequesterSimple`) is included with the library. It depends on a few basic PHP extensions: stream, url and json.

These extensions are extremely common and will most likely be available on the PHP 5 installed by your hosting company. However, the 'json' extension is only available in PHP 5.2 and above! If you need to support lower versons of PHP 5, you can create your own sub-class of HiringThingHttpRequester which uses a JSON decoder implemented in pure PHP.

You may also want to create your own sub-class of `HiringThingHttpRequester` in order to integrate with your framework.

For example, Drupal provides the function `drupal_http_request()` which is more powerful than the request code included in the default sub-class: it supports HTTP proxies, logs errors to the standard Drupal error log, retries failed requests, etc. It makes a lot of sense for a Drupal user to use a sub-class that takes advantage of that function.

The same is probably true for your framework! Whether that is Symphony or Zend or whatever. It probably provides a more powerful HTTP request mechanism than the simple included default.

Here's an example:

```php
class MyHttpRequester extends HiringThingHttpRequester {
  public function make_request($url, $data = NULL, $method = 'GET', $headers = array()) {
    // do the actual work here!

    // return the decoded JSON from the given $url
    return $data;
  }
}

// pass the name of your class as the 2nd argument to the HiringThing constructor
$hiringthing = new HiringThing($credentials, 'MyHttpRequester');
```

