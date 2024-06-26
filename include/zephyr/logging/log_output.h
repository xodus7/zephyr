/*
 * Copyright (c) 2018 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef ZEPHYR_INCLUDE_LOGGING_LOG_OUTPUT_H_
#define ZEPHYR_INCLUDE_LOGGING_LOG_OUTPUT_H_

#include <zephyr/logging/log_msg2.h>
#include <zephyr/sys/util.h>
#include <stdarg.h>
#include <zephyr/sys/atomic.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Log output API
 * @defgroup log_output Log output API
 * @ingroup logger
 * @{
 */

/** @brief Flag forcing ANSI escape code colors, red (errors), yellow
 *         (warnings).
 */
#define LOG_OUTPUT_FLAG_COLORS			BIT(0)

/** @brief Flag forcing timestamp */
#define LOG_OUTPUT_FLAG_TIMESTAMP		BIT(1)

/** @brief Flag forcing timestamp formatting. */
#define LOG_OUTPUT_FLAG_FORMAT_TIMESTAMP	BIT(2)

/** @brief Flag forcing severity level prefix. */
#define LOG_OUTPUT_FLAG_LEVEL			BIT(3)

/** @brief Flag preventing the logger from adding CR and LF characters. */
#define LOG_OUTPUT_FLAG_CRLF_NONE		BIT(4)

/** @brief Flag forcing a single LF character for line breaks. */
#define LOG_OUTPUT_FLAG_CRLF_LFONLY		BIT(5)

/** @brief Flag forcing syslog format specified in RFC 5424
 */
#define LOG_OUTPUT_FLAG_FORMAT_SYSLOG		BIT(6)

/** @brief Supported backend logging format types for use
 * with log_format_set() API to switch log format at runtime.
 */
#define LOG_OUTPUT_TEXT 0

#define LOG_OUTPUT_SYST 1

#define LOG_OUTPUT_DICT 2

/**
 * @brief Prototype of the function processing output data.
 *
 * @param buf The buffer data.
 * @param size The buffer size.
 * @param ctx User context.
 *
 * @return Number of bytes processed, dropped or discarded.
 *
 * @note If the log output function cannot process all of the data, it is
 *       its responsibility to mark them as dropped or discarded by returning
 *       the corresponding number of bytes dropped or discarded to the caller.
 */
typedef int (*log_output_func_t)(uint8_t *buf, size_t size, void *ctx);

/* @brief Control block structure for log_output instance.  */
struct log_output_control_block {
	atomic_t offset;
	void *ctx;
	const char *hostname;
};

/** @brief Log_output instance structure. */
struct log_output {
	log_output_func_t func;
	struct log_output_control_block *control_block;
	uint8_t *buf;
	size_t size;
};

/**
 * @brief Typedef of the function pointer table "format_table".
 *
 * @param output Pointer to log_output struct.
 * @param msg2 Pointer to log_msg2 struct.
 * @param flags Flags used for text formatting options.
 *
 * @return Function pointer based on Kconfigs defined for backends.
 */
typedef void (*log_format_func_t)(const struct log_output *output,
					struct log_msg2 *msg2, uint32_t flags);

/**
 * @brief Declaration of the get routine for function pointer table format_table.
 */
log_format_func_t log_format_func_t_get(uint32_t log_type);

/** @brief Create log_output instance.
 *
 * @param _name Instance name.
 * @param _func Function for processing output data.
 * @param _buf  Pointer to the output buffer.
 * @param _size Size of the output buffer.
 */
#define LOG_OUTPUT_DEFINE(_name, _func, _buf, _size)			\
	static struct log_output_control_block _name##_control_block;	\
	static const struct log_output _name = {			\
		.func = _func,						\
		.control_block = &_name##_control_block,		\
		.buf = _buf,						\
		.size = _size,						\
	}

/** @brief Process log messages v2 to readable strings.
 *
 * Function is using provided context with the buffer and output function to
 * process formatted string and output the data.
 *
 * @param log_output Pointer to the log output instance.
 * @param msg Log message.
 * @param flags Optional flags.
 */
void log_output_msg2_process(const struct log_output *log_output,
			     struct log_msg2 *msg, uint32_t flags);

/** @brief Process log messages v2 to SYS-T format.
 *
 * Function is using provided context with the buffer and output function to
 * process formatted string and output the data in sys-t log output format.
 *
 * @param log_output Pointer to the log output instance.
 * @param msg Log message.
 * @param flag Optional flags.
 */
void log_output_msg2_syst_process(const struct log_output *log_output,
			     struct log_msg2 *msg, uint32_t flag);

/** @brief Process dropped messages indication.
 *
 * Function prints error message indicating lost log messages.
 *
 * @param output Pointer to the log output instance.
 * @param cnt        Number of dropped messages.
 */
void log_output_dropped_process(const struct log_output *output, uint32_t cnt);

/** @brief Flush output buffer.
 *
 * @param output Pointer to the log output instance.
 */
void log_output_flush(const struct log_output *output);

/** @brief Function for setting user context passed to the output function.
 *
 * @param output	Pointer to the log output instance.
 * @param ctx		User context.
 */
static inline void log_output_ctx_set(const struct log_output *output,
				      void *ctx)
{
	output->control_block->ctx = ctx;
}

/** @brief Function for setting hostname of this device
 *
 * @param output	Pointer to the log output instance.
 * @param hostname	Hostname of this device
 */
static inline void log_output_hostname_set(const struct log_output *output,
					   const char *hostname)
{
	output->control_block->hostname = hostname;
}

/** @brief Set timestamp frequency.
 *
 * @param freq Frequency in Hz.
 */
void log_output_timestamp_freq_set(uint32_t freq);

/** @brief Convert timestamp of the message to us.
 *
 * @param timestamp Message timestamp
 *
 * @return Timestamp value in us.
 */
uint64_t log_output_timestamp_to_us(uint32_t timestamp);

/**
 * @}
 */


#ifdef __cplusplus
}
#endif

#endif /* ZEPHYR_INCLUDE_LOGGING_LOG_OUTPUT_H_ */
