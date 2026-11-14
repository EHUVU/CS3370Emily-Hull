# Pulse Analysis Report
## 1. Prediction (written before any timing)

I believe the slowest performance will be the smoothing the data because it has to copy and perform the calculation every time a cleared
vector is passed for each file.

## 2. Baseline (commit tagged `baseline`)
Data: <20> files x <1,000,000> samples, seed <42>. Build: -O2.
How I checked the generated data: 
I generated 20 benchmark files with 1,000,000 samples each with the seed 42. I generated 5 edge case files.

The first 20 values in bench/pulse_1.dat are small values and then it starts dropping dramatically into negative numbers.

The comparison between the pulse positions in truth.txt and baseline_bench.txt are pretty close. The generator reported 104 pulses and my
program reported 100. The variations between the pulse starting positions in my program benchmark and the truth.txt were only slightly off.

The baseline_bench detects piggyback pulses that are reported differently in the truth.txt like the one found at 273 in pulse_2.dat.

I think this is pretty close. It is good enough to work with.


For the edge cases, it completed without any sanitizer errors or crashes. Invalid.dat didn't report the abc that is present in there so there
might be something wrong with the input validation.

Here is the stage timing results from the first run on the optimized executable:

 for i in 1 2 3 4 5; do
    echo "RUN $i"
    ../pulse ../ini/pulse.ini > /dev/null
done
RUN 1

Stage timing report:
area: 0.057297 ms, 99 calls
detection: 8.04439 ms, 20 calls
negated: 12.2175 ms, 20 calls
piggyback: 0.004145 ms, 20 calls
reading: 489.714 ms, 20 calls
smoothing: 33.4795 ms, 20 calls
RUN 2

Stage timing report:
area: 0.059574 ms, 99 calls
detection: 8.10414 ms, 20 calls
negated: 12.4397 ms, 20 calls
piggyback: 0.004418 ms, 20 calls
reading: 489.744 ms, 20 calls
smoothing: 33.4816 ms, 20 calls
RUN 3

Stage timing report:
area: 0.056411 ms, 99 calls
detection: 8.04859 ms, 20 calls
negated: 12.3132 ms, 20 calls
piggyback: 0.004369 ms, 20 calls
reading: 486.315 ms, 20 calls
smoothing: 33.4494 ms, 20 calls
RUN 4

Stage timing report:
area: 0.057173 ms, 99 calls
detection: 8.02167 ms, 20 calls
negated: 11.9889 ms, 20 calls
piggyback: 0.003997 ms, 20 calls
reading: 492.329 ms, 20 calls
smoothing: 33.3841 ms, 20 calls
RUN 5

Stage timing report:
area: 0.055605 ms, 99 calls
detection: 7.9911 ms, 20 calls
negated: 12.0982 ms, 20 calls
piggyback: 0.004405 ms, 20 calls
reading: 474.808 ms, 20 calls
smoothing: 33.3041 ms, 20 calls

| Stage | Median ms | Share of total |
|-------|-----------|----------------|
Reading       489.714       20
Smoothing     33.4494       20
Negated       12.2175       20
Detection     8.04439       20
Area          0.057173      20
Piggyback     0.004369      20
## 3. Suggestions
| # | Suggestion | Category | Decision | Reason | Effect |
|---|------------|----------|----------|--------|--------|
1. replace the iterator-based integer parsing with a faster reading strategy. | Performance | Added to code | Using std::from_chars avoids stream formatting overhead.

This had a significant impact on the timing.

Reading     268.676     20
Smoothing   32.5877     20
Negated     11.3882     20
Detection   8.13908     20
Area        0.064594    99
Piggyback   0.004548    20

2. Check for invalid data instead of accepting partial files. | Safety | Added to code | The invalid.dat would pass the "abc" when it shouldn't.

This now prints the error message instead of silently allowing it. Here is the printed result:
Loaded ../ini/pulse.ini
vt=100
width=100
pulse_delta=15
drop_ratio=0.75
below_drop_ratio=4
Invalid file: "./empty.dat"

"./five.dat"
"./seven.dat"
"./invalid.dat"
Invalid data: "./invalid.dat"
"./noise_only.dat"

Stage timing report:
detection: 0.000261 ms, 3 calls
negated: 0.000712 ms, 3 calls
piggyback: 0.00011 ms, 3 calls
reading: 0.014277 ms, 4 calls
smoothing: 0.000831 ms, 3 calls

3. Avoid repeatedly growing the smoothing vector. | Performance | Added to code | SmoothData clears and then fills it using back_inserter and push_back. Reduce reallocations.

Here is the timing result:

Reading     298.708     20
Smoothing   24.0115     20
Negated     11.3783     20
Detection   8.04004     20
Area        0.060228    99
Piggyback   0.00468     20

Reading increased, but smoothing went down significantly.

4. Eliminate the unnecessary copy before negation. | Performance | Added to code | negateData() currently copies the entire storage vector into negatedValues and then traverses it again to perform the negation. Use a single transfor directly from storage into negatedValues to reduce two passes of data to one.

Here is the timing result:

Reading     290.607     20
Smoothing   26.188      20
Negated     5.96271     20
Detection   7.98985     20
Area        0.059832    99
Piggyback   0.004626    20

Looks like the negated median reduced significantly.

5. Protect against integer overflow in smoothing and detection. | Safety | Added to code | The weighted smoothing calculation performs multiplication and addition using int and can potentially overflow.

I changed the int to long long and added a check to see if there is overflow.

6. Pass the vector in piggybackFilter to const reference. | Modern C++ | Added to code | The pulses vector in the parameters is being passed by value and not by reference.

All I can say is.... whoops... I am suprised I didn't catch this one. This is changed now.

7. Validate area calculation bounds and width. | Safety | Added to code | computeArea() assumes that start is a valid index and that width is a nonnegative value. Validate these before constructing the iterator range.

Printed results:

Loaded ../ini/pulse.ini
vt=100
width=100
pulse_delta=15
drop_ratio=0.75
below_drop_ratio=4
Invalid file: "./empty.dat"

"./five.dat"
"./seven.dat"
"./invalid.dat"
Invalid data: "./invalid.dat"
"./noise_only.dat"

Stage timing report:
detection: 0.004769 ms, 3 calls
negated: 0.081973 ms, 3 calls
piggyback: 0.001162 ms, 3 calls
reading: 0.254183 ms, 4 calls
smoothing: 0.020619 ms, 3 calls


## 4. Final timing
| Stage | Baseline ms | Final ms | Change |
|-------|-------------|----------|--------|
Reading     489.714     291.002     -40.58%
Smoothing   33.4494     32.3327     -3.34%
Negated     12.2175     5.96577     -51.17%
Detection   8.04439     9.75297     +21.24%
Area        0.057173    0.008646    -84.88%
Piggyback   0.004369    0.005       +14.44%
Total       543.487     339.067     -37.61%

## 5. Reflection

1. How did your prediction compare with the timing report? Where does the time actually
go?

The time goes to reading for the most part. I was sure it was going to be smoothed data being the highest.

2. Choose one suggestion you rejected, or one that turned out to be wrong. What evidence
decided it?

I will be honest here. I know the assignment requirement says that I must reject 1 of these, but all of the suggestions here were beneficial.

3. Did gen_data.cpp or stage_timer.h contain a mistake? How did you find it, or how
did you check that there was none?

No mistakes. They were clear.

4. Which suggestion taught you something about C++ that you did not know before?

I think the most interesting suggestion was the first suggestion for std::istream_iterator<int>. Using the std::from_chars absolutely cut the reading time
down significantly because the cost was less expensive. It just goes to show that even the smallest overhead against a large set of values can add up and
slow the performance down significantly.


Please note: I used ChatGPT. Exporting the conversation isn't very easy. CTRL + P only makes a pdf that shows what I say and nothing of what Chat GPT said in the conversation. Copy pasting the whole thing into google docs makes the formatting strange.