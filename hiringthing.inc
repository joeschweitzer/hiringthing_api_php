<?php

define('HIRINGTHING_VERSION', '1.0');

/**
 * User friendly layer for accessing the HiringThing API.
 */
class HiringThing {
  protected $requester;

  /**
   * Constructs a HiringThing object.
   *
   * @param array $credentials
   *   An associative array containing:
   *     - subdomain: Your subdomain from http://subdomain.hiringthing.com
   *     - api_key: Your API key
   *     - api_password: You API password
   * @param string $requester_class
   *   The name of a sub-class of HiringThingHttpRequester to use to make HTTP
   *   requests.
   *
   */
  public function __construct($credentials, $requester_class = 'HiringThingHttpRequesterSimple') {
    if (is_object($credentials) && is_a($credentials, 'HiringThingHttpRequester')) {
      // if the user passed a HiringThingHttpRequester in directly
      $this->requester = $credentials;
    }
    else {
      // we create a new requester from the given class
      $this->requester = new $requester_class($credentials);
    }
  }

  /**
   * Get a list of jobs.
   *
   * @param string $type
   *   (optional) The type of jobs to return. Can be one of: all, active,
   *   hidden, archived.
   *
   * @return array
   *   An array of HiringThingJob objects.
   */
  public function getJobs($type = 'all') {
    if (!in_array($type, array('all', 'active', 'hidden', 'archived'))) {
      throw new HiringThingException("Invalid jobs type: $type");
    }

    $path = 'jobs';
    if ($type != 'all') {
      $path .= '/' . $type;
    }

    $data = $this->requester->request($path);

    // convert the result to an array of HiringThingJobs
    foreach ($data as $idx => $obj) {
      $data[$idx] = new HiringThingJob($this->requester, $obj);
    }

    return $data;
  }

  /**
   * Get a list of applications.
   *
   * @param string $type
   *   (optional) The type of applications to return. Can be one of: all,
   *   urated, rated, archived.
   *
   * @return array
   *   An array of HiringThingApplication objects.
   */
  public function getApplications($type = 'all') {
    if (!in_array($type, array('all', 'unrated', 'rated', 'archived'))) {
      throw new HiringThingException("Invalid application type: $type");
    }

    $path = 'applications';
    if ($type != 'all') {
      $path .= '/' . $type;
    }

    $data = $this->requester->request($path);

    // convert the result to an array of HiringThingApplications
    foreach ($data as $idx => $obj) {
      $data[$idx] = new HiringThingApplication($this->requester, $obj);
    }

    return $data;
  }

  /**
   * Get a particular job by id.
   *
   * @param int $id
   *   The id of the job to get.
   *
   * @returns HiringThingJob
   *   The object representing the given job.
   */
  public function getJob($id) {
    $path = "jobs/$id";
    $data = $this->requester->request($path);
    return new HiringThingJob($this->requester, $data[0]);
  }

  /**
   * Get a particular application by id.
   *
   * @param int $id
   *   The id of the application to get.
   *
   * @returns HiringThingApplication
   *   The object representing the given application.
   */
  public function getApplication($id) {
    $path = "applications/$id";
    $data = $this->requester->request($path);
    return new HiringThingApplication($this->requester, $data);
  }
}

/**
 * Represents a job.
 */
class HiringThingJob extends HiringThingObject {
  /**
   * Get a list of applications.
   *
   * @param string $type
   *   (optional) The type of applications to return. Can be one of: all,
   *   urated, rated, archived.
   *
   * @return array
   *   An array of HiringThingApplication objects.
   */
  public function getApplications($type = 'all') {
    if (!in_array($type, array('all', 'unrated', 'rated', 'archived'))) {
      throw new HiringThingException("Invalid application type: $type");
    }

    $path = "jobs/{$this->id}/applications";
    if ($type != 'all') {
      $path .= '/' . $type;
    }

    $data = $this->requester->request($path);

    // convert the result to an array of HiringThingApplications
    foreach ($data as $idx => $obj) {
      $data[$idx] = new HiringThingApplication($this->requester, $obj);
    }

    return $data;
  }
  
  /**
   * Creates a new application for this job
   *
   * @param array $applicationData An associative array containing the application data
   * you wish to create (must contain at least first name, last name, phone, and email).
   *
   * @returns Associative array of response (will contain success => true) if application
   * was created successfully.
   */
  public function createApplication($applicationData) {
    $path = "jobs/{$this->id}/applications";
    $headers = array('Content-Type' => 'application/json');
    
    $data = $this->requester->request($path, json_encode($applicationData), 'POST', $headers);
    
    return $data;
  }
}

/**
 * Represents a job application.
 */
class HiringThingApplication extends HiringThingObject {
}

/**
 * Parent of the HiringThingJob and HiringThingApplication classes.
 */
class HiringThingObject {
  protected $requester;
  protected $data;

  function __construct($requester, $data = array()) {
    $this->requester = $requester;
    $this->data = (array)$data;
  }

  function __get($name) {
    if (isset($this->data[$name])) {
      return $this->data[$name];
    }

    // This isn't an exception, it's a non-fatal error.
    $class = get_class($this);
    trigger_error("Undefined property $class->$name", E_USER_NOTICE);

    return NULL;
  }

  function __isset($name) {
    return isset($this->data[$name]);
  }
}

/**
 * Exception class for all exceptions generated by this library.
 */
class HiringThingException extends Exception {
}

/**
 * Abstract class for making HTTP requests.
 *
 * You can extend this to hook into the HTTP request mechanism provided by your
 * library or framework by overriding the make_request() method.
 */
abstract class HiringThingHttpRequester {
  protected $subdomain;
  protected $api_key;
  protected $api_password;
  protected $auth_headers;

  public function __construct($credentials) {
    foreach (array('subdomain', 'api_key', 'api_password') as $name) {
      if (empty($credentials[$name])) {
        throw new HiringThingException("credentials array must contain '$name'");
      }
      $this->$name = $credentials[$name];
    }
  }

  /**
   * Makes an HTTP request to a particular path on the HiringThing API endpoint.
   *
   * @param string $path
   *   Path relative to the base HiringThing endpoint. For example: 'jobs' or
   *   'jobs/active' or 'jobs/111/applications/unrated'.
   * @param array|string|null $data
   *   (optional) An array or string representing the data to be passed with the
   *   request. If it's an array, it will be URL encoded. If it's a string it
   *   will be passed as the request body.
   * @param string $method
   *   (optional) The HTTP method to use. Defaults to 'GET'.
   * @param array $headers
   *   (optional) Additional HTTP headers to use in the request.
   *
   * @returns object|array
   *   The decoded JSON object or array returned in response to the request.
   */
  public function request($path, $data = NULL, $method = 'GET', $headers = array()) {
    // normalize arguments
    $headers = array_merge($headers, $this->get_auth_headers());
    $method = strtoupper($method);
    $url = 'https://' . $this->subdomain . '.hiringthing.com/remote/' . $path . '.json';

    // pass it off to the child class
    return $this->make_request($url, $data, $method, $headers);
  }

  /**
   * Get th headers for HTTP Basic Auth.
   */
  protected function get_auth_headers() {
    if (empty($this->auth_headers)) {
      $value = $this->api_key . ':' . $this->api_password;
      $this->auth_headers = array(
        'Authorization' => 'Basic ' . base64_encode($value),
      );
    }
    return $this->auth_headers;
  }

  /**
   * Abstract function for actually making the HTTP request.
   *
   * Override this function in a sub-class to change how HTTP requests are made.
   *
   * @param string $url
   *   Absolute URL to make the request to.
   * @param array|string|null $data
   *   (optional) An array or string representing the data to be passed with the
   *   request. If it's an array, it should be URL encoded. If it's a string it
   *   should be passed as the request body.
   * @param string $method
   *   (optional) The HTTP method to use. Defaults to 'GET'. This is normalized
   *   always be all capitals.
   * @param array $headers
   *   (optional) Additional HTTP headers to use in the request (already
   *   includes HTTP headers to authenticate against the HiringThing API).
   *
   * @returns object|array
   *   The decoded JSON object or array returned in response to the request.
   */
  abstract protected function make_request($url, $data = NULL, $method = 'GET', $headers = array());
}

/**
 * A super simple and portable implementation of HiringThingHttpRequester.
 *
 * It only requires the 'stream', 'url' and 'json' extensions which are
 * very common. It doesn't depend on any PHP libraries or frameworks.
 */
class HiringThingHttpRequesterSimple extends HiringThingHttpRequester {
  protected function make_request($url, $data = NULL, $method = 'GET', $headers = array()) {
    // convert $data from an array if necessary
    if (is_array($data)) {
      $data = http_build_query($data);
      if ($method == 'GET') {
        $url .= '?' . $data;
        $data = NULL;
      }
    }

    // put headers into the format expected by stream_context_create()
    foreach ($headers as $name => $value) {
      $headers[$name] = "$name: $value";
    }
    $headers = implode("\r\n", $headers);

    $context = stream_context_create(array(
      'http' => array(
        'method' => $method,
        'header' => $headers,
        'content' => is_null($data) ? '': $data,
      ),
    ));

    $data = file_get_contents($url, false, $context);
    return json_decode($data);
  }
}

