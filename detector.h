/*

                                COPYRIGHT
    Copyright (C) 2015 Kyle Briggs (kbrig035<at>uottawa.ca)

    This file is part of CUSUM.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef DETECTOR_H_INCLUDED
#define DETECTOR_H_INCLUDED
#include"utils.h"
#include"io.h"
#include"bessel.h"
#include"stepfit.h"
#include<inttypes.h>
#include<stdint.h>
#include<string.h>
#include<math.h>
#include<stdio.h>
#include<stdlib.h>
#include<float.h>
#define HISTOGRAM 0
#define FIRST_DERIV 1
#define SCND_DERIV 2
int64_t fit_events(configuration *config, io_struct *io, double *rawsignal, event *current_event, bessel *lpfilter, edge *current_edge, int64_t *error_summary, int64_t edgecount, timestruct *timestats);
edge *find_edges(configuration *config, io_struct *io, signal_struct *sig, baseline_struct *baseline_stats, bessel *lpfilter, edge *current_edge, edge *head_edge);
int64_t get_next_event(event *current_event, edge *current_edge, int64_t index);
int64_t get_next_event_start(edge *current_edge);

void calculate_level_noise(event *current, int64_t minpoints);
void populate_event_levels(event *current);
void identify_step_events(event *current, int64_t stepfit_samples, int64_t subevent_minpoints, int attempt_recovery);



void filter_short_events(event *current, int64_t minpoints);
void filter_long_events(event *current, int64_t event_maxpoints);


int64_t average_cusum_levels(event *current, int64_t subevent_minpoints, double cusum_minstep, int attempt_recovery, int64_t padding_wait);
void cusum(event *current_event, double delta, double minthreshold, double maxthreshold, int64_t subevent_minpoints, int64_t padding_wait, timestruct *timestats, double elasticity);
double get_cusum_threshold(int64_t length, double minthreshold, double maxthreshold, double sigma, double mun);

void refine_event_estimates(event *current);

void event_max_blockage(event *current, double minstep);

void count_crossing(event *current, double intra_threshold, double intra_hysteresis);

edge *detect_edges(double *signal, double baseline, int64_t length, edge *current, double threshold, double stdev, double hysteresis, int64_t position, int event_direction);
void generate_trace(FILE *input, event *current, int datatype, void *rawsignal, FILE *logfile, bessel *lpfilter, int eventfilter, chimera *daqsetup, edge *current_edge, int64_t last_end, int64_t start, int64_t subevent_minpoints, double savegain, int64_t padding_wait, chimera_file *chimera_input);




void event_baseline(event *current_event, double baseline_min, double baseline_max);
void event_area(event *current_event, double timestep);

void estimate_time_statistics(duration_struct *current_duration, timestruct *timestats, edge *current_edge);
int64_t get_durations(duration_struct *current_duration, edge *current_edge);
void gauss_histogram(double *signal, baseline_struct *baseline, int64_t length);
void fit_gaussian(baseline_struct *baseline);
#endif // DETECTOR_H_INCLUDED
