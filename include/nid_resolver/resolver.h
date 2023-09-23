#pragma once

#include <stddef.h>
#include <elf.h>

#ifdef __cplusplus
extern "C" {
#define restrict __restrict
#endif

typedef struct {
	size_t impl[3];
} Resolver;

/**
 * @brief Create and initializes resolver object
 *
 * @return Resolver
 */
Resolver create_resolver(void);

/**
 * @brief Initializes a resolver object
 */
void init_resolver(Resolver *restrict self);

/**
 * @brief Destroy the resolver object
 *
 * @param self the resolver object
 */
void destroy_resolver(Resolver *restrict self);

/**
 * @brief Moves the resolver object
 *
 * @param self the resolver object
 * @param rhs the resolver object to be moved
 */
void move_resolver(Resolver *restrict self, Resolver *restrict rhs);

/**
 * @brief Reserve memory for a specified number of libraries
 *
 * @param self the resolver object
 * @param num_libraries the number of libraries to reserve memory for
 * @return 0 on success, non-zero on error
 */
int reserve_library_memory(Resolver *restrict self, size_t num_libraries);

/**
 * @brief Add a library to the resolver
 *
 * @param self the resolver object
 * @param imagebase the library image base
 * @param symtab the library symbol table
 * @param symtab_length the length of the symbol table
 * @param strtab the library string table
 * @return 0 on success, non-zero on error
 */
int add_library(Resolver *restrict self, uintptr_t imagebase, const Elf64_Sym *restrict symtab, size_t symtab_length, const char *restrict strtab);

/**
 * @brief Add a library to the resolver
 *
 * @param self the resolver object
 * @param imagebase the library image base
 * @param app_meta the kernel address of the metadata for the library
 * @return 0 on success, non-zero on error
 */
int add_library_metadata(Resolver *restrict self, uintptr_t imagebase, uintptr_t app_meta);

/**
 * @brief Lookup a symbol
 *
 * @param self the resolver object
 * @param sym the symbol to lookup
 * @param length the length of the symbol
 * @return the symbol virtual address if found otherwise 0
 */
uintptr_t lookup_symbol(const Resolver *restrict self, const char *restrict sym, size_t length);


#ifdef __cplusplus
}
/**
 * @brief RAII wrapper for the Resolver object
 *
 */
class ManagedResolver : public Resolver {

	public:
		ManagedResolver() noexcept {
			init_resolver(this);
		}
		ManagedResolver(const ManagedResolver&) = delete;
		ManagedResolver operator=(const ManagedResolver&) = delete;
		ManagedResolver(ManagedResolver &&rhs) noexcept : Resolver((Resolver&&)rhs) {
			__builtin_memset(&rhs, 0, sizeof(rhs));
		}
		ManagedResolver &operator=(ManagedResolver &&rhs) noexcept {
			move_resolver(this, &rhs);
			return *this;
		}
		~ManagedResolver() noexcept {
			destroy_resolver(this);
		}
		/*! @copydoc ::reserve_library_memory(Resolver *restrict, size_t) */
		int reserve_library_memory(size_t num_libraries) noexcept {
			return ::reserve_library_memory(this, num_libraries);
		}
		/*! @copydoc ::add_library(Resolver *restrict, const uintptr_t, const Elf64_Sym *restrict, const size_t, const char *restrict) */
		int add_library(const uintptr_t imagebase, const Elf64_Sym *restrict symtab, const size_t symtab_length, const char *restrict strtab) noexcept {
			return ::add_library(this, imagebase, symtab, symtab_length, strtab);
		}
		/*! @copydoc ::add_library_metadata(Resolver *restrict, const uintptr_t, const uintptr_t) */
		int add_library_metadata(const uintptr_t imagebase, const uintptr_t app_meta) noexcept {
			return ::add_library_metadata(this, imagebase, app_meta);
		}
		/*! @copydoc ::lookup_symbol(const Resolver *restrict, const char *restrict) */
		uintptr_t lookup_symbol(const char *restrict sym, size_t length = 0) noexcept {
			if (length == 0) {
				length = strlen(sym);
			}
			return ::lookup_symbol(this, sym, length);
		}
};

#undef restrict
#endif
