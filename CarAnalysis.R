install.packages("gganimate")
install.packages("gifski")     # For rendering GIFs
install.packages("transformr") # For smooth transitions
install.packages("tidyverse")  # includes dplyr, tidyr, ggplot2, etc.

library(gganimate)
library(transformr)
library(tidyr)
library(ggplot2)
library(dplyr)

install.packages("gganimate")
# Read the data (assuming space-delimited and no header)
data <- read.table("C:\\Users\\Shivansh\\OneDrive\\Desktop\\serial_20250416_124652.txt", header = FALSE, sep = ",")

# Assign column names
colnames(data) <- c("Timestamp", "left_distance", "right_distance", "rpm", "direction_turned")

# Write to CSV with the new column names
write.csv(data, "C:\\Users\\Shivansh\\OneDrive\\Desktop\\newfile22.csv", row.names = FALSE)
data1<-read.csv("C:\\Users\\Shivansh\\OneDrive\\Desktop\\newfile22.csv")

#convert timestamp to proper format
data1$Timestamp <- as.POSIXct(data1$Timestamp, format = "%H:%M:%OS", tz = "UTC")

# Line Plot – RPM over Time
plot(data1$Timestamp, data1$rpm, type = "l", col = "blue", xlab = "Time", ylab = "RPM")

# Line Plot – Left vs Right Distance
plot(data1$Timestamp, data1$left_distance, type = "l", col = "red", xlab = "Time", ylab = "Distance")
lines(data1$Timestamp, data1$right_distance, col = "green")
legend("topright", legend = c("Left", "Right"), col = c("red", "green"), lty = 1)

# Count frequencies
dir_counts <- table(data1$direction_turned)

# Choose a color for each direction
bar_colors <- c("skyblue", "orange", "lightgreen")  # Add more if needed

# Make sure the number of colors matches the number of categories
barplot(dir_counts, col = bar_colors, main = "Turn Direction Frequency")

# Add a legend
legend("topleft", legend = names(dir_counts), fill = bar_colors)

#Left distance vs right distance vs direction turned vs Timestamp
ggplot(data1, aes(x = left_distance, y = right_distance)) +
  geom_point(aes(color = direction_turned, size = Timestamp), alpha = 0.8) +
  geom_line(aes(group = 1), linewidth = 1, alpha = 0.4) +
  labs(title = "Left vs Right Distance vs Direction Turned vs Time",
       x = "Left Distance (cm)",
       y = "Right Distance (cm)",
       color = "Direction Turned",
       size = "Time") +
  theme_minimal()


#animation of left distance vs right distance over Timestamp
data_long <- data1 %>%
  pivot_longer(cols = c(left_distance, right_distance),
               names_to = "side",
               values_to = "distance")
p <- ggplot(data_long, aes(x = Timestamp, y = distance, color = side)) +
  geom_line(size = 1) +
  labs(title = "Left vs Right Distance Over Time",
       x = "Time",
       y = "Distance (cm)",
       color = "Side") +
  theme_minimal() +
  transition_reveal(Timestamp)
animate(p, width = 800, height = 500, fps = 20, duration = 10, renderer = gifski_renderer())

# Optional: Save it
anim_save("left_vs_right_distance.gif")



