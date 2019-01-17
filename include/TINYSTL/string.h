/*-
 * Copyright 2012-2018 Matthew Endsley
 * All rights reserved
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted providing that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef TINYSTL_STRING_H
#define TINYSTL_STRING_H

#include <initializer_list>

#include <TINYSTL/allocator.h>
#include <TINYSTL/buffer.h>
#include <TINYSTL/stddef.h>
#include <TINYSTL/hash.h>

namespace tinystl {

	template<typename T>
	inline constexpr T min(const T& a, const T& b) { return a < b ? a : b; }

	template<typename T>
	inline constexpr T max(const T& a, const T& b) { return a > b ? a : b; }

	template<typename Alloc>
	class basic_string {
	public:
		basic_string();
		basic_string(const basic_string& other);
		//basic_string(const basic_string& other, size_t pos, size_t len = npos);
		basic_string(basic_string&& other);
		basic_string(const char* s);
		basic_string(const char* s, size_t len);
		basic_string(size_t len, char c);
		basic_string(const char* first, const char* last);
		basic_string(std::initializer_list<char> list);
		~basic_string();

		basic_string& operator=(const basic_string& other);
		basic_string& operator=(basic_string&& other);

		const char* data() const;
		char* data();

		const char* c_str() const;
		size_t size() const;
		size_t capacity() const;
		bool empty() const;

		char& operator[](size_t idx);
		const char& operator[](size_t idx) const;

		const char& front() const;
		char& front();
		const char& back() const;
		char& back();

		void resize(size_t size);
		void resize(size_t size, char value);
		void clear();
		void reserve(size_t capacity);

		void push_back(char t);
		void pop_back();

		void shrink_to_fit();

		void swap(basic_string& other);

		typedef char value_type;

		typedef char* pointer;
		typedef char* iterator;
		iterator begin();
		iterator end();

		typedef const char* const_iterator;
		const_iterator begin() const;
		const_iterator end() const;

		void append(const char* first, const char* last);
		void append(const basic_string& other);
		void assign(const char* first, const char* last);
		void assign(const char* s, size_t n);
		void insert(size_t where, char c);
		void insert(size_t where, const char* first, const char* last);
		void insert(size_t where, const basic_string& other);
		void insert(iterator where, char c);
		void insert(iterator where, const char* first, const char* last);
		void insert(iterator where, const basic_string& other);

		basic_string& operator+=(const basic_string& other);

		void erase(size_t pos = 0, size_t len = npos);
		iterator erase(iterator where);
		iterator erase(iterator first, iterator last);

		basic_string substr(size_t begin, size_t count = npos) const;

		size_t find(char c, const size_t offset = 0) const;
		size_t rfind(char c, const size_t offset = npos) const;
		size_t find(const basic_string& other, const size_t offset = 0) const;
		size_t rfind(const basic_string& other, const size_t offset = npos) const;

		int compare(const basic_string& other) const;

		static constexpr auto npos{ static_cast<size_t>(-1) };

	protected:
		static const size_t c_nbuffer = 12;
		char m_small[12];

		buffer<char, Alloc> m_buffer;
	};

	template<typename Alloc>
	inline basic_string<Alloc>::basic_string()
		: m_buffer{ m_small, m_small, m_small + c_nbuffer }
	{}

	template<typename Alloc>
	inline basic_string<Alloc>::basic_string(const basic_string& other)
		: basic_string()
	{
		string_reserve(m_buffer, m_small, other.size());
		string_copy(m_buffer, other.m_buffer.first, other.m_buffer.last);
	}

	template<typename Alloc>
	inline basic_string<Alloc>::basic_string(basic_string&& other)
		: basic_string()
	{
		if(other.m_buffer.first == other.m_small)
			string_copy(m_buffer, other.m_buffer.first, other.m_buffer.last);
		else
			m_buffer = other.m_buffer;
		string_reset(other.m_buffer, other.m_small, c_nbuffer);
	}

	template<typename Alloc>
	inline basic_string<Alloc>::basic_string(const char* s)
		: basic_string()
	{
		size_t len = 0;
		for(const char* it = s; *it; ++it)
			++len;

		string_reserve(m_buffer, m_small, len);
		string_copy(m_buffer, s, s + len);
	}

	template<typename Alloc>
	inline basic_string<Alloc>::basic_string(const char* s, size_t len)
		: basic_string()
	{
		string_reserve(m_buffer, m_small, len);
		string_copy(m_buffer, s, s + len);
	}

	template<typename Alloc>
	inline basic_string<Alloc>::basic_string(size_t len, char c)
		: basic_string()
	{
		string_reserve(m_buffer, m_small, len);
		buffer_fill_urange(m_buffer.first, m_buffer.last, c);
	}

	template<typename Alloc>
	inline basic_string<Alloc>::basic_string(const char* first, const char* last) 
		: basic_string()
	{
		string_reserve(m_buffer, m_small, last - first);
		string_copy(m_buffer, first, last);
	}

	template<typename Alloc>
	inline basic_string<Alloc>::basic_string(std::initializer_list<char> list) 
		: basic_string(list.begin(), list.end())
	{
	}

	template<typename Alloc>
	inline basic_string<Alloc>::~basic_string() {
		if(m_buffer.first != m_small)
			Alloc::static_deallocate(m_buffer.first, m_buffer.capacity - m_buffer.first);
	}

	template<typename Alloc>
	inline basic_string<Alloc>& basic_string<Alloc>::operator=(const basic_string& other) {
		basic_string(other).swap(*this);
		return *this;
	}

	template<typename Alloc>
	basic_string<Alloc>& basic_string<Alloc>::operator=(basic_string&& other) {
		basic_string(static_cast<basic_string&&>(other)).swap(*this);
		return *this;
	}

	template<typename Alloc>
	inline const char* basic_string<Alloc>::data() const {
		return m_buffer.first;
	}

	template<typename Alloc>
	inline char* basic_string<Alloc>::data() {
		return m_buffer.first;
	}

	template<typename Alloc>
	inline const char* basic_string<Alloc>::c_str() const {
		return m_buffer.first;
	}

	template<typename Alloc>
	inline size_t basic_string<Alloc>::size() const {
		return (size_t)(m_buffer.last - m_buffer.first);
	}

	template<typename Alloc>
	inline size_t basic_string<Alloc>::capacity() const {
		return (size_t)(m_buffer.capacity - m_buffer.first);
	}

	template<typename Alloc>
	inline bool basic_string<Alloc>::empty() const {
		return m_buffer.last == m_buffer.first;
	}

	template<typename Alloc>
	inline char& basic_string<Alloc>::operator[](size_t idx) {
		return m_buffer.first[idx];
	}

	template<typename Alloc>
	inline const char& basic_string<Alloc>::operator[](size_t idx) const {
		return m_buffer.first[idx];
	}

	template<typename Alloc>
	inline const char& basic_string<Alloc>::front() const {
		return m_buffer.first[0];
	}

	template<typename Alloc>
	inline char& basic_string<Alloc>::front() {
		return m_buffer.first[0];
	}

	template<typename Alloc>
	inline const char& basic_string<Alloc>::back() const {
		return m_buffer.last[-1];
	}

	template<typename Alloc>
	inline char& basic_string<Alloc>::back() {
		return m_buffer.last[-1];
	}

	template<typename Alloc>
	inline void basic_string<Alloc>::resize(size_t size) {
		string_resize(m_buffer, m_small, size);
	}

	template<typename Alloc>
	inline void basic_string<Alloc>::resize(size_t size, char value) {
		string_resize(m_buffer, m_small, size, value);
	}

	template<typename Alloc>
	inline void basic_string<Alloc>::clear() {
		string_clear(m_buffer);
	}

	template<typename Alloc>
	inline void basic_string<Alloc>::reserve(size_t capacity) {
		string_reserve(m_buffer, m_small, capacity);
	}

	template<typename Alloc>
	inline void basic_string<Alloc>::push_back(char c) {
		buffer_grow_count(m_buffer, 1, 1, m_buffer.first == m_small);
		*m_buffer.last++ = c;
		*m_buffer.last = 0;
	}

	template<typename Alloc>
	inline void basic_string<Alloc>::pop_back() {
		string_pop(m_buffer);
	}

	template<typename Alloc>
	inline void basic_string<Alloc>::swap(basic_string& other) {
		const pointer tfirst = m_buffer.first, tlast = m_buffer.last, tcapacity = m_buffer.capacity;
		m_buffer.first = other.m_buffer.first, m_buffer.last = other.m_buffer.last, m_buffer.capacity = other.m_buffer.capacity;
		other.m_buffer.first = tfirst, other.m_buffer.last = tlast, other.m_buffer.capacity = tcapacity;

		char tbuffer[c_nbuffer];

		if(m_buffer.first == other.m_small)
			for(pointer it = other.m_small, end = m_buffer.last, out = tbuffer; it != end; ++it, ++out)
				*out = *it;

		if(other.m_buffer.first == m_small) {
			string_reset(other.m_buffer, other.m_small, c_nbuffer, other.m_buffer.last - other.m_buffer.first);
			for(pointer it = other.m_buffer.first, end = other.m_buffer.last, in = m_small; it != end; ++it, ++in)
				*it = *in;
			*other.m_buffer.last = 0;
		}

		if(m_buffer.first == other.m_small) {
			string_reset(m_buffer, m_small, c_nbuffer, m_buffer.last - m_buffer.first);
			for(pointer it = m_buffer.first, end = m_buffer.last, in = tbuffer; it != end; ++it, ++in)
				*it = *in;
			*m_buffer.last = 0;
		}
	}

	template<typename Alloc>
	inline void basic_string<Alloc>::shrink_to_fit() {
		if(m_buffer.first != m_small)
			buffer_shrink_to_fit(m_buffer, 1);
	}

	template<typename Alloc>
	inline typename basic_string<Alloc>::iterator basic_string<Alloc>::begin() {
		return m_buffer.first;
	}

	template<typename Alloc>
	inline typename basic_string<Alloc>::iterator basic_string<Alloc>::end() {
		return m_buffer.last;
	}

	template<typename Alloc>
	inline typename basic_string<Alloc>::const_iterator basic_string<Alloc>::begin() const {
		return m_buffer.first;
	}

	template<typename Alloc>
	inline typename basic_string<Alloc>::const_iterator basic_string<Alloc>::end() const {
		return m_buffer.last;
	}

	template<typename Alloc>
	inline void basic_string<Alloc>::append(const char* first, const char* last) {
		string_append(m_buffer, m_small, first, last);
	}

	template<typename Alloc>
	inline void basic_string<Alloc>::append(const basic_string& other) {
		string_append(m_buffer, m_small, other.begin(), other.end());
	}

	template<typename Alloc>
	inline void basic_string<Alloc>::assign(const char* first, const char* last) {
		string_clear(m_buffer);
		string_append(m_buffer, m_small, first, last);
	}

	template<typename Alloc>
	inline void basic_string<Alloc>::assign(const char* sz, size_t n) {
		string_clear(m_buffer);
		string_append(m_buffer, m_small, sz, sz + n);
	}

	template<typename Alloc>
	inline void basic_string<Alloc>::insert(size_t where, char value) {
		string_insert(m_buffer, m_small, m_buffer.first + where, value);
	}

	template<typename Alloc>
	inline void basic_string<Alloc>::insert(size_t where, const char* first, const char* last) {
		string_insert(m_buffer, m_small, m_buffer.first + where, first, last);
	}

	template<typename Alloc>
	inline void basic_string<Alloc>::insert(size_t where, const basic_string& other) {
		string_insert(m_buffer, m_small, m_buffer.first + where, other.begin(), other.end());
	}

	template<typename Alloc>
	inline void basic_string<Alloc>::insert(iterator where, char value) {
		string_insert(m_buffer, m_small, where, value);
	}

	template<typename Alloc>
	inline void basic_string<Alloc>::insert(iterator where, const char* first, const char* last) {
		string_insert(m_buffer, m_small, where, first, last);
	}

	template<typename Alloc>
	inline void basic_string<Alloc>::insert(iterator where, const basic_string& other) {
		string_insert(m_buffer, m_small, where, other.begin(), other.end());
	}

	template<typename Alloc>
	inline basic_string<Alloc>& basic_string<Alloc>::operator+=(const basic_string& other) {
		string_append(m_buffer, m_small, other.begin(), other.end());
		return *this;
	}

	template<typename Alloc>
	inline void basic_string<Alloc>::erase(size_t pos, size_t len) {
		string_erase(m_buffer, m_buffer.first + pos, len == npos ? m_buffer.last : m_buffer.first + pos + len);
	}

	template<typename Alloc>
	inline typename basic_string<Alloc>::iterator basic_string<Alloc>::erase(iterator where) {
		return string_erase(m_buffer, where, where + 1);
	}

	template<typename Alloc>
	inline typename basic_string<Alloc>::iterator basic_string<Alloc>::erase(iterator first, iterator last) {
		return string_erase(m_buffer, first, last);
	}

	template<typename Alloc>
	inline basic_string<Alloc> basic_string<Alloc>::substr(size_t begin, size_t count) const {
		return basic_string(m_buffer.first + begin, min(count, this->size() - begin));
	}

	template<typename Alloc>
	inline int basic_string<Alloc>::compare(const basic_string& other) const {
		pointer first1 = m_buffer.first;
		pointer first2 = other.m_buffer.first;
		const pointer last1 = m_buffer.last;
		const pointer last2 = other.m_buffer.last;

		const size_t count1 = size_t(last1 - first1);
		const size_t count2 = size_t(last2 - first2);
		size_t count = count1 < count2 ? count1 : count2;

		for(; 0 < count; --count, ++first1, ++first2)
			if(*first1 != *first2)
			{
				return (*first1 < *first2 ? -1 : +1);
			}

		if(count1 < count2) return -1;
		if(count1 > count2) return 1;
		return 0;
	}

	inline int string_compare(const char* first1, const char* first2, size_t count)
	{
		for(; 0 < count; --count, ++first1, ++first2)
			if(*first1 != *first2)
			{
				return *first1 < *first2 ? -1 : 1;
			}
		return 0;
	}

	inline const char* string_find(const char* first, size_t count, const char& c)
	{
		for(; 0 < count; --count, ++first)
			if(*first == c)
			{
				return first;
			}
		return nullptr;
	}

	template<typename Alloc>
	inline size_t basic_string<Alloc>::find(char c, const size_t offset) const {
		const size_t hay_size = size();
		const char* hay = m_buffer.first;
		if(offset < hay_size)
		{
			const char* match = string_find(hay + offset, hay_size - offset, c);
			if(match)
				return size_t(match - m_buffer.first);
		}

		return size_t(-1);
	}

	template<typename Alloc>
	inline size_t basic_string<Alloc>::rfind(char c, const size_t offset) const {
		const size_t hay_size = size();
		const char* hay = m_buffer.first;
		if(hay_size != 0)
			for(const char* current = hay + min(offset, hay_size - 1); current != hay; --current)
			{
				if(*current == c)
					return size_t(current - hay);
			}

		return size_t(-1);
	}

	template<typename Alloc>
	inline size_t basic_string<Alloc>::find(const basic_string& other, const size_t offset) const {
		const char* hay = this->begin();
		const char* needle = other.begin();
		const size_t hay_size = size();
		const size_t needle_size = other.size();

		if(needle_size > hay_size || offset > hay_size - needle_size)
			return size_t(-1);
		if(needle_size == 0)
			return offset;

		const char* last_possible = hay + (hay_size - needle_size) + 1;
		for(const char* current = hay + offset; ; ++current)
		{
			current = string_find(current, size_t(last_possible - current), *needle);
			if(!current)
				return size_t(-1);
			if(string_compare(current, needle, needle_size) == 0)
				return size_t(current - hay);
		}
	}

	template<typename Alloc>
	inline size_t basic_string<Alloc>::rfind(const basic_string& other, const size_t offset) const {
		const char* hay = this->begin();
		const char* needle = other.begin();
		const size_t hay_size = this->size();
		const size_t needle_size = other.size();

		if(needle_size == 0)
			return min(offset, hay_size);

		if(needle_size <= hay_size)
			for(const char* current = hay + min(offset, hay_size - needle_size); current != hay; --current)
			{
				if(*current == *needle && string_compare(current, needle, needle_size) == 0)
				{
					return size_t(current - hay);
				}
			}

		return size_t(-1);
	}

	template<typename LAlloc, typename RAlloc>
	inline bool operator==(const basic_string<LAlloc>& lhs, const basic_string<RAlloc>& rhs) {
		typedef const char* pointer;

		const size_t lsize = lhs.size(), rsize = rhs.size();
		if (lsize != rsize)
			return false;

		pointer lit = lhs.c_str(), rit = rhs.c_str();
		pointer lend = lit + lsize;
		while (lit != lend)
			if (*lit++ != *rit++)
				return false;

		return true;
	}

	template<typename LAlloc, typename RAlloc>
	inline bool operator<(const basic_string<LAlloc>& lhs, const basic_string<RAlloc>& rhs) {
		return lhs.compare(rhs) < 0;
	}

	template<typename LAlloc, typename RAlloc>
	inline bool operator!=(const basic_string<LAlloc>& lhs, const basic_string<RAlloc>& rhs) { return !(lhs == rhs); }

	template<typename Alloc>
	inline bool operator==(const basic_string<Alloc>& lhs, const char* rhs) { return lhs == basic_string<Alloc>(rhs); }

	template<typename Alloc>
	inline bool operator==(const char* lhs, const basic_string<Alloc>& rhs) { return basic_string<Alloc>(lhs) == rhs; }
	
	template<typename Alloc>
	inline bool operator!=(const basic_string<Alloc>& lhs, const char* rhs) { return lhs != basic_string<Alloc>(rhs); }

	template<typename Alloc>
	inline bool operator!=(const char* lhs, const basic_string<Alloc>& rhs) { return basic_string<Alloc>(lhs) != rhs; }

	template<typename Alloc>
	static inline size_t hash(const basic_string<Alloc>& value) {
		return hash_string(value.c_str(), value.size());
	}

	typedef basic_string<TINYSTL_ALLOCATOR> string;
}

#endif
