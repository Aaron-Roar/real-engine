#!/usr/bin/env perl
use strict;
use warnings;

my ($header, $out) = @ARGV;

die "usage: $0 include/rohr.h docs/public_api.md\n" unless defined $header && defined $out;

open my $fh, '<', $header or die "open $header: $!";
local $/;
my $text = <$fh>;
close $fh;

my @entries;
while ($text =~ m{/\*\*(.*?)\*/}sg) {
    my $comment = $1;
    next if $comment =~ /^\s*\*\s*\@file\b/m || $comment =~ /\@file\b/;

    my $after = substr($text, pos($text));
    $after =~ s/^\s+//;
    next if $after =~ m{^/\*\*};

    my @sig_lines;
    for my $line (split /\n/, $after) {
        $line =~ s/^\s+|\s+$//g;
        next if $line eq '';
        last if $line =~ m{^/\*\*};
        push @sig_lines, $line;
        last if $line =~ /;$/ || $line =~ /^#define/;
    }
    next unless @sig_lines;

    my $sig = join ' ', @sig_lines;
    next unless $sig =~ /\brohr_[A-Za-z0-9_]+\b/;

    my @comment_lines;
    for my $line (split /\n/, $comment) {
        $line =~ s/^\s*\* ?//;
        $line =~ s/\s+$//;
        push @comment_lines, $line;
    }

    my ($brief, $ret) = ('', '');
    my @details;
    my @params;
    my $mode = 'details';
    for my $line (@comment_lines) {
        next if $line =~ /^\s*$/;
        if ($line =~ /^\@brief\s*(.*)/) {
            $brief = $1;
            $mode = 'details';
            next;
        }
        if ($line =~ /^\@param\s+(\S+)\s*(.*)/) {
            push @params, [$1, $2];
            $mode = 'param';
            next;
        }
        if ($line =~ /^\@return\s*(.*)/) {
            $ret = $1;
            $mode = 'return';
            next;
        }
        next if $line =~ /^\@/;
        if ($mode eq 'param' && @params) {
            $params[-1]->[1] .= ' ' . $line;
        } elsif ($mode eq 'return') {
            $ret .= ' ' . $line;
        } else {
            push @details, $line;
        }
    }

    my ($name) = $sig =~ /\b(rohr_[A-Za-z0-9_]+)\b/;
    push @entries, {
        name => $name,
        signature => $sig,
        brief => $brief,
        details => \@details,
        params => \@params,
        ret => $ret,
    };
}

sub group_name {
    my ($name) = @_;
    return 'Engine' if $name =~ /^rohr_engine_/;
    return 'Errors and Results' if $name =~ /^rohr_error_/;
    return 'Console' if $name =~ /^rohr_console_/;
    return 'Entities' if $name =~ /^rohr_entity_/;
    return 'Physics' if $name =~ /^rohr_physics_/;
    return 'Graphics' if $name =~ /^rohr_graphics_/;
    return 'Math' if $name =~ /^rohr_math_/;
    return 'Systems' if $name =~ /^rohr_system_/;
    return 'Level Editor' if $name =~ /^rohr_level_editor_/;
    return 'Controller Input' if $name =~ /^rohr_controller_/;
    return 'Spatial Grid' if $name =~ /^rohr_grid_/;
    return 'Tools' if $name =~ /^rohr_tools_/;
    return 'Other';
}

sub anchor {
    my ($s) = @_;
    $s = lc $s;
    $s =~ s/[^a-z0-9 ]//g;
    $s =~ s/\s+/-/g;
    return $s;
}

my @order = (
    'Engine',
    'Errors and Results',
    'Console',
    'Entities',
    'Physics',
    'Graphics',
    'Math',
    'Systems',
    'Level Editor',
    'Controller Input',
    'Spatial Grid',
    'Tools',
    'Other',
);
my %groups = map { $_ => [] } @order;

for my $entry (@entries) {
    push @{ $groups{group_name($entry->{name})} }, $entry;
}

my @lines;
push @lines, '# Public API Reference', '';
push @lines, 'This page is the GitHub-readable reference for the public Rohr Engine C API.';
push @lines, 'The source of truth is [`include/rohr.h`](../include/rohr.h), which uses Doxygen comments for the generated HTML documentation.', '';
push @lines, 'Application code should include the public facade:', '';
push @lines, '```c', '#include "rohr.h"', '```', '';
push @lines, 'Entity values are stable ids, not component table indexes. Use the public entity functions to validate ids and resolve indexes.', '';
push @lines, '## Contents', '';

for my $group (@order) {
    next unless @{ $groups{$group} };
    push @lines, '- <a href="#' . anchor($group) . '">' . $group . '</a>';
}
push @lines, '';

for my $group (@order) {
    next unless @{ $groups{$group} };
    push @lines, '## ' . $group, '';
    for my $entry (@{ $groups{$group} }) {
        push @lines, '### `' . $entry->{name} . '`', '';
        push @lines, '```c', $entry->{signature}, '```', '';
        push @lines, $entry->{brief}, '' if $entry->{brief} ne '';
        for my $detail (@{ $entry->{details} }) {
            push @lines, $detail, '';
        }
        if (@{ $entry->{params} }) {
            push @lines, '| Parameter | Description |', '| --- | --- |';
            for my $param (@{ $entry->{params} }) {
                push @lines, '| `' . $param->[0] . '` | ' . $param->[1] . ' |';
            }
            push @lines, '';
        }
        push @lines, '**Returns:** ' . $entry->{ret}, '' if $entry->{ret} ne '';
    }
}

open my $outfh, '>', $out or die "open $out: $!";
print $outfh join("\n", @lines);
close $outfh;
