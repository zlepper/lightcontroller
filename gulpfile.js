var gulp = require('gulp'),
    gutil = require('gulp-util'),
    uglify = require("gulp-uglify"),
    bowersource = "bower_components/",

    jshint = require('gulp-jshint'),
    sass = require('gulp-sass'),
    concat = require('gulp-concat'),
    sourcemaps = require('gulp-sourcemaps'),
    ownScripts = 'source/javascript/**/*.js',

    input = {
        sass: [
            bowersource + "angular-material/angular-material.scss",
            'source/scss/**/*.scss'
        ],

        javascript: [
            bowersource + "angular/angular.js",
            bowersource + "angular-animate/angular-animate.js",
            bowersource + "angular-aria/angular-aria.js",
            bowersource + "angular-messages/angular-messages.js",
            bowersource + "angular-resource/angular-resource.js",
            bowersource + "angular-material/angular-material.js",
            bowersource + "angular-ui-router/release/angular-ui-router.js",
            "source/javascript/main.js",
            ownScripts
        ]
    },

    output = "public";


/* run the watch task when gulp is called without arguments */
gulp.task('default', ['build-css', 'build-js', 'watch']);

/* run javascript through jshint */
gulp.task('jshint', function () {
    "use strict";
    return gulp.src(ownScripts)
        .pipe(jshint())
        .pipe(jshint.reporter('jshint-stylish'));
});

/* compile scss files */
gulp.task('build-css', function () {
    "use strict";
    return gulp.src(input.sass)
        .pipe(sourcemaps.init())
        .pipe(sass())
        .pipe(concat("bundle.css"))
        .pipe(sourcemaps.write())
        .pipe(gulp.dest(output));
});

/* concat javascript files, minify if --type production */
gulp.task('build-js', function () {
    "use strict";
    return gulp.src( input.javascript )
        .pipe(sourcemaps.init())
        .pipe(concat('bundle.js'))
        //only uglify if gulp is ran with '--type production'
        .pipe(gutil.env.type === 'production' ? uglify() : gutil.noop())
        .pipe(sourcemaps.write())
        .pipe(gulp.dest(output));
});

/* Watch these files for changes and run the task on update */
gulp.task('watch', function () {
    "use strict";
    gulp.watch(ownScripts, ['jshint', 'build-js']);
    gulp.watch(input.sass, ['build-css']);
});
