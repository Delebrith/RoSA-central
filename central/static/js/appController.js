app.factory("CORSInterceptor", [
    function()
    {
        return {
            request: function(config)
            {
                config.headers["Access-Control-Allow-Origin"] = "*";
                config.headers["Access-Control-Allow-Methods"] = "GET, POST, DELETE, OPTIONS";
                config.headers["Access-Control-Allow-Headers"] = "Content-Type, access-control-allow-headers, " +
                    "access-control-allow-origin, access-control-allow-methods, Accept";
                config.headers["Access-Control-Request-Headers"] = "X-Requested-With, accept, content-type";
                return config;
            }
        };
    }
]);

app.config(["$httpProvider", function ($httpProvider) {
    $httpProvider.interceptors.push("CORSInterceptor");
}]);

app.controller("appController", function($scope, $http, $cookies, $location) {
	$scope.context = {
			user: undefined
	}

	$scope.isLogged = function () {
		return $cookies.get("session_id") != null
	}

	$scope.serverAddress = "http://" + self.location.hostname + ":8081";

});