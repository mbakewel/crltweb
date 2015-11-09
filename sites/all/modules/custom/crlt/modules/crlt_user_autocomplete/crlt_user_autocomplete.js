/**
 * Javascript override of Drupal.ACDB.prototype.search
 * From misc/autocomplete.js
 * Replaces Javascript alert() with console.log() when $.ajax fails for any reason
 *
 * Wait until JQuery's ready event to override the prototype.
 * That way, it works regardless of Javascript load order.
 */
$(document).ready(function() {
	/**
	 * Performs a cached and delayed search
	 */
	Drupal.ACDB.prototype.search = function(searchString) {
		var db = this;
		this.searchString = searchString;

		// See if this key has been searched for before
		if (this.cache[searchString]) {
			return this.owner.found(this.cache[searchString]);
		}

		// Initiate delayed search
		if (this.timer) {
			clearTimeout(this.timer);
		}
		this.timer = setTimeout(function() {
			db.owner.setStatus('begin');

			// Ajax GET request for autocompletion
			$.ajax({
				type: "GET",
				url: db.uri + '/' + Drupal.encodeURIComponent(searchString),
				dataType: 'json',
				success: function(matches) {
					if (typeof matches['status'] == 'undefined' || matches['status'] !== 0) {
						db.cache[searchString] = matches;
						// Verify if these are still the matches the user wants to see
						if (db.searchString == searchString) {
							db.owner.found(matches);
						}
						db.owner.setStatus('found');
					}
				},
				error: function(xmlhttp) {
					console.log(Drupal.ahahError(xmlhttp, db.uri));
				}
			});
		}, this.delay);
	};

});